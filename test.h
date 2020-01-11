typedef union {
    unsigned char all;
    struct {
        unsigned SENS                       :1; //Hall sense state
        unsigned NOTDEF1                    :1; //Nodef1
        unsigned NOTDEF2                    :1; //Nodef2
        unsigned NOTDEF3                    :1; //Nodef3
        unsigned SELECT1                    :1; //Select table ignition #1
        unsigned SELECT2                    :1; //Select table ignition #2
        unsigned FUNC1                      :1; //Engine stop
        unsigned FUNC2                      :1; //Warm spark
    };
} STATE_TOGGLE_t;

STATE_TOGGLE_t Port = {0x00};

// Store flags
typedef union {
    unsigned char all;
    struct {
        unsigned lastState                    :1; //Last state Hall sensor
        unsigned overflowCount                    :1; //Overflow count
        unsigned coilOff                      :1; //Coil disable when engine not work
        unsigned engineStop                      :1; //Disable engine
        unsigned overrun1                    :1; //Enable limit overrun FUNC1
        unsigned overrun2                    :1; //Enable limit overrun FUNC2
        unsigned NOTDEF1                    :1; //Nodef1
        unsigned NOTDEF2                    :1; //Nodef2
    };
} FLAG_t;

FLAG_t Flag = {0x00};

//Emulation state of PIN
typedef struct {
        unsigned IGN_BLOCK_OUT :1;
        unsigned HALL_INPUT :1;
        unsigned LED_SHADOW :1;
        unsigned NOTDEF1 :5;
} PIN_STATE_t;

PIN_STATE_t PinState = {0x00};

uint8_t countSELECT1 = 0;
uint8_t countSELECT2 = 0;
uint8_t countFUNC1 = 0;
uint8_t countFUNC2 = 0;
uint8_t countHALL = 0; //счетчик фильтра датчика Холла
uint8_t sectorCount = 0; //Счетчик отсчетов прерываний по таймеру при проходе шторки через датчик Холла
uint8_t lastSectionCount = 0; //Последнее значение счетчик отсчетов прерываний по таймер при проходе шторки через датчик Холла
uint8_t sparkTime = 0; //Кол-во отсчетов после которого происходит включение катушки при нахождении шторки в датчике Холла
const uint8_t countEnought = 255;
const uint8_t countHallEnought = 4;
uint16_t coilCount = 0;
uint16_t coilOffCount = 0;

// Read empty byte from SPI SDI
uint8_t readEmpty;

// Table shift ignition
//uint8_t shiftIgn = 0;
//uint8_t shiftIgnCount = 0;
uint8_t shiftIgnMassive[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            7, 8, 9 ,9 ,10 ,10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18,
                            19, 19, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 31,
                            31, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 44, 45,
                            46, 46, 47, 48, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59,
                            60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
                            75, 76, 77, 78, 79, 80, 81, 82, 82, 83, 84, 85, 86, 87, 88, 89,
                            90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 102, 103, 104,
                            105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118,
                            119, 120, 121, 122, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
                            132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 141, 143, 144, 146,
                            147, 148, 150, 151, 152, 154, 155, 156, 158, 159, 160, 162, 163, 164,
                            166, 167, 168, 170, 171, 172, 174, 175, 177, 178, 179, 181, 182, 183,
                            185, 186, 187, 189, 190, 191, 193, 194, 195, 197, 198, 199, 201, 202,
                            203, 205, 206, 208, 209, 210, 212, 213, 214, 216, 217, 218, 220, 221,
                            222, 224, 225, 226, 228, 229, 230, 232, 233, 234, 236, 237, 238, 000};

void TMR1_DefaultInterruptHandler(void);
void printPinState();
void printFlag();
void printPort();