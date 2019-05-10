#include "server_utils.h"

bool decode_message(recv_msg_t* received, tcp_msg_t* to_send) {
    CHECK(received->type > 3, "Incorrect publisher message type.\n")

    long long int_num;
    double real_num;

    strncpy(to_send->topic_name, received->topic_name, 50);
    to_send->topic_name[50] = 0;

    switch (received->type) {
        case 0:
            // s-a primit un INT
            CHECK(received->data[0] > 1, "Incorrect publisher sign byte.\n")
            int_num = ntohl(*(uint32_t*)(received->data + 1));

            if (received->data[0]) {
                int_num *= -1;
            }

            sprintf(to_send->data, "%lld", int_num);
            strcpy(to_send->type, "INT");
            break;

        case 1:
            // s-a primit un SHORT_REAL
            real_num = ntohs(*(uint16_t*)(received->data));
            real_num /= 100;
            sprintf(to_send->data, "%.2f", real_num);
            strcpy(to_send->type, "SHORT_REAL");
            break;

        case 2:
            // s-a primit un FLOAT
            CHECK(received->data[0] > 1, "Incorrect publisher sign byte.\n")

            real_num = ntohl(*(uint32_t*)(received->data + 1));
            real_num /= pow(10, received->data[5]);

            if (received->data[0]) {
                real_num *= -1;
            }

            sprintf(to_send->data, "%lf", real_num);
            strcpy(to_send->type, "FLOAT");
            break;

        default:
            // s-a primit un STRING
            strcpy(to_send->type, "STRING");
            strcpy(to_send->data, received->data);
            break;
    }

    return true;
}

void update_max_fd(int& max_fd, fd_set* active_fds) {
    for (int j = max_fd; j > 2; --j) {
        if(FD_ISSET(j, active_fds)) {
            max_fd = j;
            break;
        }
    }
}

void close_all(fd_set *fds, int max_fd) {
    for (int i = 2; i <= max_fd; ++i) {
        if (FD_ISSET(i, fds)) {
            close(i);
        }
    }
}

void unsubscribe(int fd, char* topic_name, unordered_map<string, unordered_set<int>>& online,
        unordered_map<int, client_t>& clients) {
    auto it = online.find(topic_name);

    if (it != online.end() && it->second.find(fd) != it->second.end()) {
        // topicul cerut exista si clientul era abonat la topic
        it->second.erase(fd);
        clients[fd].topics.erase(topic_name);
    }
}

void subscribe(int fd, serv_msg_t* serv_msg, unordered_map<string, unordered_set<int>>& online,
        unordered_map<int, client_t>& clients) {
    // se adauga file descriptorul clientului in hash-ul pentru clientii online si se updateaza
    // SF-ul topicului
    online[serv_msg->topic_name].insert(fd);
    clients[fd].topics[serv_msg->topic_name] = serv_msg->sf;
}

void unsubscribe_all(int fd, unordered_map<string, unordered_set<int>>& online,
        unordered_map<int, client_t>& clients, unordered_map<string, vector<tcp_msg_t>>& buffers,
        unordered_map<string, vector<topic_t>>& client_topics,
        unordered_map<string, int>& clients_per_topic) {
    string crt_id = clients[fd].id;
    string topic_name;

    // se sterg toate topicurile clientului din map-ul de clienti online si se retin pentru o
    // conectare viitoare
    for (auto& crt_topic : clients[fd].topics) {
        topic_name = crt_topic.topic;

        // se sterge clientul din lista topicului sau intregul topic daca singurul client abonat
        // la acesta era cel curent
        if (online[topic_name].size() == 1) {
            online.erase(topic_name);
        } else {
            online[topic_name].erase(fd);
        }

        // se retin topicurile la care clientul era abonat, tinandu-se seama de SF
        if (crt_topic.SF) {
            client_topics[crt_id].emplace_back(true, buffers[topic_name].size(),
                                               topic_name);
            ++clients_per_topic[crt_topic.topic];
        } else {
            client_topics[crt_id].emplace_back(false, -1, topic_name);
        }
    }

    clients.erase(fd);
}

void update_buffer(int index, string& topic_name, unordered_map<string, vector<tcp_msg_t>>& buffers,
        unordered_map<string, vector<topic_t>>& client_topics) {
    int min_index = index;

    // se cauta indexul minim de la care se vor trimite mesajele din buffer
    for (auto& entry : client_topics) {
        for (topic_t& crt_topic : entry.topic_data) {
            if (crt_topic.topic_name == topic_name && crt_topic.last_msg < min_index) {
                min_index = crt_topic.last_msg;
            }
        }
    }

    if (min_index == 0) {
        return;
    }

    // se sterg mesajele pana la acest index minim
    vector<tcp_msg_t>& crt_buffer = buffers[topic_name];
    crt_buffer.erase(crt_buffer.begin(), crt_buffer.begin() + min_index);

    // se updateaza noii indecsi de la care se incepe trimiterea mesajelor bufferate dupa
    // reconectari
    for (auto& entry : client_topics) {
        for (topic_t& crt_topic : entry.topic_data) {
            if (crt_topic.topic_name == topic_name) {
                crt_topic.last_msg -= min_index;
            }
        }
    }
}

void send_buffered_messages(int fd, char* id, unordered_map<string, unordered_set<int>>& online,
        unordered_map<string, vector<topic_t>>& client_topics,
        unordered_map<string, vector<tcp_msg_t>>& buffers,
        unordered_map<string, int>& clients_per_topic, client_t& crt_client) {
    int len;

    // se iau pe rand topicurile bufferate si se readauga clientului
    // in cazul in care topicurile aveau SF setat pe 1, se retrimit mesajele bufferate in timpul
    // in care clientul a fost offline
    for (auto& buffered_topic : client_topics[id]) {
        if (buffered_topic.sf) {
            vector<tcp_msg_t>& crt_buffer = buffers[buffered_topic.topic_name];
            len = crt_buffer.size();

            // se retrimit mesajele bufferate
            for (int j = buffered_topic.last_msg; j < len; ++j) {
                ASSERT(send(fd, (char*) &crt_buffer[j], sizeof(tcp_msg_t), 0) < 0,
                       "Unable to send message to TCP client")
            }

            // se scade numarul de clienti ce au bufferat mesaje pe acest topic
            // daca numarul ajunge la 0, se sterge intreaga intrare din map-ul de buffere
            if(!--clients_per_topic[buffered_topic.topic_name]) {
                buffers.erase(buffered_topic.topic_name);
            } else {
                // daca mai sunt clienti ce au bufferat mesaje pe acest topic, se sterg, daca se
                // poate , mesajele ce nu mai intereseaza pe niciunul dintre clienti
                update_buffer(buffered_topic.last_msg, buffered_topic.topic_name, buffers,
                              client_topics);
            }
        }

        // se adauga topicul in informatiile despre client si file descriptorul noului client in
        // mapul clentilor online
        crt_client.topics.insert({buffered_topic.topic_name, buffered_topic.sf});
        online[buffered_topic.topic_name].insert(fd);
    }
}