#include <iostream>
#include <random>

class AtmMachine;
class WaitingState;
class PinVerificationState;
class OperationState;

class State{
public:
    virtual void insert_card(){};
    virtual void check_pin(const std::string& password){};
    virtual void withdraw_money(float amount){};
    virtual void deposit_money(float amount){};
    virtual void complete_work(){};
};

class WaitingState: public State{
    AtmMachine* atmMachine;
public:
    explicit WaitingState(AtmMachine* machine): State(){
        atmMachine = machine;
    }
    void insert_card();
};

class PinVerificationState: public State{
    AtmMachine* atmMachine;
public:
    explicit PinVerificationState(AtmMachine* machine): State(){
        atmMachine = machine;
    }
    void check_pin(const std::string& password);
    void complete_work();
};

class OperationState: public State{
    AtmMachine* atmMachine;
public:
    explicit OperationState(AtmMachine* machine): State(){
        atmMachine = machine;
    }
    void withdraw_money(float amount);
    void deposit_money(float amount);
    void complete_work();
};

class AtmMachine{ //Класс банкомата (он же Context)
    double total_money;
    float probability;

    State* currentState;
    WaitingState* atmWaitingState;
    PinVerificationState* atmPinVerificationState;
    OperationState* atmOperationState;
public:
    AtmMachine(double t_m, float p): total_money(t_m), probability(p){
        atmWaitingState = new WaitingState(this);
        atmPinVerificationState = new PinVerificationState(this);
        atmOperationState = new OperationState(this);

        currentState = atmWaitingState;
    }
    ~AtmMachine(){
        delete atmWaitingState;
        delete atmPinVerificationState;
        delete atmOperationState;
    }

    WaitingState* get_waiting_state(){
        return atmWaitingState;
    }
    PinVerificationState* get_verification_state(){
        return atmPinVerificationState;
    }
    OperationState* get_operation_state(){
        return atmOperationState;
    }

    void setState(State* state){
        currentState = state;
    }

    double get_balance() const{
        return total_money;
    }

    double decrease_balance(const double& n_b){
        total_money -= n_b;
    }

    float get_probability() const{
        return probability;
    }

    void insert_card();
    void check_pin(const std::string& password);
    void withdraw_money(float amount);
    void deposit_money(float amount);
    void complete_work();

};

void WaitingState::insert_card() {
    std::cout << "Card inserted!" << std::endl;
    atmMachine->setState(atmMachine->get_verification_state());
}

void PinVerificationState::check_pin(const std::string &password) {
    std::uniform_int_distribution<> distribution(0, 100);
    std::random_device rd;
    std::mt19937 gen(rd());

    int number = distribution(gen); //Генерируем случайное число
    if (number >= static_cast<int>(atmMachine->get_probability() * 100)){
        std::cout << "PIN code entered successfully!" << std::endl;
        atmMachine->setState(atmMachine->get_operation_state()); //Соединение успешно -> переходим к состоянию операции
        return;
    }
    std::cout << "Error: lost connection with server" << std::endl;
    atmMachine->setState(atmMachine->get_waiting_state()); //Ошибка -> возвращаемся к состоянию ожидания
}

void PinVerificationState::complete_work() {
    atmMachine->setState(atmMachine->get_waiting_state());
    std::cout << "Work completed!" << std::endl;
}

void OperationState::withdraw_money(float amount) {
    if (amount <= atmMachine->get_balance()){
        std::cout << "Money taken!" << std::endl;
        atmMachine->decrease_balance(amount);
        return;
    }
    std::cout << "Not enough money!" << std::endl;
}

void OperationState::deposit_money(float amount) {
    std::cout << "Account has been successfully funded!" << std::endl;
}

void OperationState::complete_work() {
    std::cout << "Work completed!" << std::endl;
    atmMachine->setState(atmMachine->get_waiting_state());
}

void AtmMachine::insert_card() {
    currentState->insert_card();
}

void AtmMachine::check_pin(const std::string &password) {
    currentState->check_pin(password);
}

void AtmMachine::withdraw_money(float amount) {
    currentState->withdraw_money(amount);
}

void AtmMachine::deposit_money(float amount) {
    currentState->deposit_money(amount);
}

void AtmMachine::complete_work() {
    currentState->complete_work();
}



int main() {
    AtmMachine bank(1000, 0.1);
    bank.insert_card();
    bank.check_pin("hello");
    bank.withdraw_money(100);
    bank.deposit_money(150);
    bank.complete_work();

    return 0;
}
