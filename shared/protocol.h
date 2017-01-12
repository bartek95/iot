#include <string>
#include <sstream>

#define BUFFER_SIZE 256
#define SLEEP_TIME 1

// TODO: dopisać więcej typów
enum SensorType {
    TEMPERATURE,
};

struct SensorData {
    SensorType sensor_type;
    int value;
};

struct SensorDataStream {
    char buffer[BUFFER_SIZE];
    int begin;
    int end;
    SensorData now;
    SensorDataStream() : begin(0), end(0) { }
    SensorData get() { return now; }
    int read_data(nRF24L01P *radio, int pipe);
    int next(nRF24L01P *radio, int pipe);
};

int SensorDataStream::read_data(nRF24L01P *radio, int pipe) {
    if (begin != 0) {
        for (int i = begin; i < end; ++i) {
            buffer[i - begin] = buffer[i];
        }
        end -= begin;
        begin = 0;
    }
    int res = 0;
    if (radio->readable(pipe)) {
        res = radio->read(pipe, buffer + end, sizeof(SensorData));
    }
    if (res < 0) return -1;
    end += res;
    return res;
}

int SensorDataStream::next(nRF24L01P *radio, int pipe) {
    while (end - begin < sizeof(SensorData)) {
        int res = read_data(radio, pipe);
        if (res < 0) return -1;
    }
    
    SensorData *temp = (SensorData*) (buffer + begin);
    //memcpy(&(stream->now), temp, sizeof(SensorData));
    now.sensor_type = temp->sensor_type;
    now.value = temp->value;
    begin += sizeof(SensorData);
    return 0;
}

// TODO: ogarniać więcej typów
std::string sensor_type_to_string(SensorType type) {
    switch (type) {
        case TEMPERATURE:
            return "temp";
        default:
            return "unknown";
    }
}