#include "mbed.h"
#include "LCD.h";

int zeit = 0;
int parkplaetze = 0;
InterruptIn einfahrt(PA_1);
InterruptIn ausfahrt(PA_6);
lcd mylcd;


void anzeigen(int anzahl) {
    mylcd.clear();
    mylcd.cursorpos(0);
    mylcd.printf("Parkplätze: %d", anzahl);
}

void zeigeParkplaetzeAn() {
    anzeigen(parkplaetze);
}

void zaehleHoch() {
    TIM7->SR = 0;
    zeit++;
    
    if (zeit == 30) {
        zeit = 0;
        einfahrt.enable_irq();
        TIM7->CR1 = 0;
        TIM6->CR1 = 1;
    }
}

void zaehleHochTIM6() {
    TIM6->SR = 0;
    zeit++;
    
    if (zeit == 10) {
        zeit = 0;
        einfahrt.disable_irq();
        TIM6->CR1 = 0;
        TIM7->CR1 = 1;
    }
}

void initTIM7() {    
    RCC->APB1ENR |= 0b100000;
    TIM7->PSC = 31999;
    TIM7->ARR = 60000;
    TIM7->CNT = 0;
    TIM7->SR = 0;
    TIM7->CR1 = 1;
    
    TIM7->DIER = 1;

    NVIC_SetVector(TIM7_IRQn, (uint32_t)&zaehleHoch);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void initTIM6() {
    RCC->APB1ENR |= 0b100000;
    TIM6->PSC = 31999;
    TIM6->ARR = 60000;
    TIM6->CNT = 0;
    TIM6->SR = 0;
    TIM6->CR1 = 0;
    
    TIM7->DIER = 1;

    NVIC_SetVector(TIM6_IRQn, (uint32_t)&zaehleHochTIM6);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);
}

void einfahrtISR() {
    parkplaetze++;
    anzeigen(parkplaetze);
}

void ausfahrtISR() {
    if (parkplaetze != 0) {
        parkplaetze--;
        anzeigen(parkplaetze);
    }
}

int main()
{
    einfahrt.mode(PullDown);
    ausfahrt.mode(PullDown);

    einfahrt.rise(&einfahrtISR);
    ausfahrt.rise(&ausfahrtISR);

    einfahrt.disable_irq();
    ausfahrt.enable_irq();

    initTIM6();
    initTIM7();

    while (true) {

    }
}

