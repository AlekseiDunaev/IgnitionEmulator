#include "../inc/main.h"

TASK_t tasks[] = {
    {40, SHADOW_IN},
    {40, SHADOW_IN},
    {40, SHADOW_IN},
    {50, SHADOW_IN},
    {50, SHADOW_IN},
    {50, SHADOW_IN},
    {60, SHADOW_IN},
    {60, SHADOW_IN},
    {60, SHADOW_IN},
    {70, SHADOW_IN},
    {70, SHADOW_IN},
    {70, SHADOW_IN},
    {80, SHADOW_IN},
    {80, SHADOW_IN},
    {80, SHADOW_IN},
    {90, SHADOW_IN},
    {90, SHADOW_IN},
    {90, SHADOW_IN},
    {100, SHADOW_IN},
    {100, SHADOW_IN},
    {100, SHADOW_IN},
    {110, SHADOW_IN},
    {110, SHADOW_IN},
    {110, SHADOW_IN},
    {150, SHADOW_IN},
    {150, SHADOW_IN},
    {150, SHADOW_IN},
    {200, SHADOW_IN},
    {200, SHADOW_IN},
    {200, SHADOW_IN},
    {220, SHADOW_IN},
    {220, SHADOW_IN},
    {220, SHADOW_IN},
    {230, SHADOW_IN},
    {230, SHADOW_IN},
    {230, SHADOW_IN},
    {240, SHADOW_IN},
    {240, SHADOW_IN},
    {240, SHADOW_IN}
};

uint8_t numberOfTasks = sizeof(tasks)/sizeof(TASK_t);

int main(int argc, char* argv[]) {
    emulationWork(tasks, numberOfTasks);
}
    
bool HALL_INPUT_GetValue() {
    return PinState.HALL_INPUT;
}

void IGN_BLOCK_OUT_SetHigh() {
    PinState.IGN_BLOCK_OUT = true;
}

void IGN_BLOCK_OUT_SetLow() {
    PinState.IGN_BLOCK_OUT = false;
}

void LED_SHADOW_SetHigh() {
    PinState.LED_SHADOW = true;
}

void LED_SHADOW_SetLow() {
    PinState.LED_SHADOW = false;
}

void TMR1_DefaultInterruptHandler(void){
    if (HALL_INPUT_GetValue() == Port.SENS) {
        countHALL = 0;
    } else {
        countHALL++;        
        if (countHALL == countHallEnought) {
            Port.SENS = !Port.SENS;
            countHALL = 0;
        }
    }
    
    if (Port.SENS) {
        //Если шторка в датчике 
        if (!Flag.lastState) { 
            //Если перед этим шторка была не в датчике
            //Шторка вошла в датчик
            if (!Flag.engineStop && !Flag.overrun1 && Flag.overrun2) {
                //Если двигатель не остановлен
                //Если нет ограничения оборотов по функции 1
                //Если нет отграниченияоборотов по функции 2
                //Включаем катушку зажигания
                IGN_BLOCK_OUT_SetHigh();
            } 
            //Сохраняем счетчик оборотов
            lastSectionCount = sectorCount;
            sectorCount = 0;
            //Обновляем сотояние нахождения шторки
            Flag.lastState = 1;
            //Сбрасываем флаг переполнения
            Flag.overflowCount = 0;
            //Обнуляем счетчик отключения катушки
            coilOffCount &= 0x0F;
            //Снимаем флаг отключения катушки. Двигатель работает.
            Flag.coilOff = 0;
            return;
            
        } else {
            //Шторка находится по прежнему в датчике
            LED_SHADOW_SetLow();//Включаем светодиод
            //Если счетчик переполнен, не проверяем его на момент искрообразования
            if (Flag.overflowCount) {
                return;
            } else {
                sectorCount++; //Увеличиваем значение счетчика срабатывания таймера
                
                if (sectorCount >= 239) { //Проверяем счетчик на перполнение
                    Flag.overflowCount = 1;
                }
                
                if (!sparkTime) { //Проверяем счетчик на момент искрообразования, если он не равен нулю (т.е. при запуске))
                    return; //Происходит запуск, искра будет образована при выходе шторки из датчика
                }
                
                if (sectorCount == sparkTime) {//Если кол-во отсчетов с начала захода шторки в датчик совпадает с расчетным, то инициализируем искру
                    IGN_BLOCK_OUT_SetLow();
                }
                
                return;
            }
        }
        
    } else if (Flag.lastState) {
        //Шторка вышла из сенсора
        IGN_BLOCK_OUT_SetLow(); //Инициализируем искру, если этого еще не произошло (запуск).
        Flag.lastState = 0;
        coilCount = 0;
        
        if (sparkTime == 0) {
            coilCount = sectorCount + 239;
        } else {
            coilCount = sectorCount + sparkTime;
        }
        
        if (sectorCount > 240) {
            sparkTime = 0;
            return;
        }
        sparkTime = shiftIgnMassive[sectorCount];

        return;
        
    } else {
        //Шторка по прежнему вне сенсора
        LED_SHADOW_SetHigh();
        if (!(--coilCount)) {
            if (Flag.engineStop || Flag.coilOff || Flag.overrun1 || Flag.overrun2) {
                return;
            }
            IGN_BLOCK_OUT_SetHigh();
        }
    }
}

void printPinState() {
    printf("IGN\tHALL\tLED\n");
    printf("%d\t%d\t%d\n\n", PinState.IGN_BLOCK_OUT, PinState.HALL_INPUT, PinState.LED_SHADOW);
}

void printFlag() {
    printf("SENS\tSELECT1\tSELECT2\tFUNC1\tFUNC2\n");
    printf("%d\t%d\t%d\t%d\t%d\n", Port.SENS, Port.SELECT1, Port.SELECT2, Port.FUNC1, Port.FUNC2);
}

void printPort() {
    printf("lastState\toverflowCount\tcoilOff\tengineStop\n");
    printf("%d\t\t%d\t\t%d\t%d\n", Flag.lastState, Flag.overflowCount, Flag.coilOff, Flag.engineStop);
}

void printAll() {
    printPort();
    printFlag();
    printPinState();
}

void emulationCicle(uint16_t timesCount) {

    int timesCountIdle = timesCount * 6;
    PinState.HALL_INPUT = SHADOW_IN;

    while (timesCount--) {
        TMR1_DefaultInterruptHandler();
    }

    printf("Flag.overloadCount: %d, sectorCount: %d.\t sparkTime: %d\n", Flag.overflowCount, sectorCount, sparkTime);

    PinState.HALL_INPUT = SHADOW_OUT;
    timesCount *= 6;

    while (timesCountIdle--) {
        TMR1_DefaultInterruptHandler();
    }

}

void emulationWork(TASK_t *tasks, uint8_t numberOfTasks) {
    for (uint8_t i = 0; i < numberOfTasks; i++) {
        emulationCicle(tasks[i].timesTMR1Interupt);
    }
}

void printShiftIgnMassive() {
    uint8_t lenghtForCicle = sizeof(shiftIgnMassive)/sizeof(uint8_t);
    uint8_t lenght = lenghtForCicle;
    while(lenghtForCicle--) {
        uint8_t i = lenght - lenghtForCicle - 1 ;
        printf("index: %d\t, value: %d\n", i, shiftIgnMassive[i]);
    }
}
