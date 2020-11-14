
enum Flags
{
    UNKNOWN = 1 << 0,
    HELP = 1 << 1,
    LIST = 1 << 2
};

void help();
void listDev();
int readArgs(int argc, char* argv[], int& device_id, int& buffer_size, int& sample_rate, int& k1, int& k2, int& k3, int& k4);