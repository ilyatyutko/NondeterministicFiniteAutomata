#include <iostream>
#include <map>
#include <list>
#include <queue>
#include <string>
#include <algorithm>

using Condition = int;
using Position = unsigned int;
using Symbol = char;


//описание класса НКА
class NonDeterminedAutomato{
private:
    //описание класса конкретной возможной ветви перехода НКА
    class ExactlyAutomatoBranching{
    public:
        Condition CurrentCondition;
        Position StartingPosition;
        Position CurrentPosition;

        ExactlyAutomatoBranching(Position starting, Position current, Condition cond)
                : CurrentCondition(cond)
                , CurrentPosition(current)
                , StartingPosition(starting){}
    };

    //Функция перехода НКА
    const std::multimap<std::pair<Condition, Symbol>, Condition> SwitchingFunction;
    //Начальное состояние
    const Condition StartingCondition;
    //Множество конечных состояний
    const std::list<Condition> FinishingConditions;

    //множество самоклонирующихся ветвей автомата
    std::queue<ExactlyAutomatoBranching> NonDeterminedAutomatosList;
    //множество слов, удовлетворяющих условию
    std::list<std::pair<Position, std::string>> AppropriateSubStrings;

    //проверка, перешла ли определенная ветвь в конечное состояние
    auto CheckIfFinishingCondition(const ExactlyAutomatoBranching& automatoBranching) const -> bool{
        return std::find(FinishingConditions.begin()
                         , FinishingConditions.end()
                         , automatoBranching.CurrentCondition) != FinishingConditions.end();
    }

    //берем произвольную ветвь
    //выполнение проверка ветви на конечное состояние + выполнение разветвления автомата,
    // если несколько возможных переходов
    auto Step(const std::string& CheckingString) -> void{
        if (NonDeterminedAutomatosList.empty())
            return;
        auto branching = NonDeterminedAutomatosList.front();
        NonDeterminedAutomatosList.pop();

        if (CheckIfFinishingCondition(branching))
            AppropriateSubStrings.push_back(
                    std::make_pair(branching.StartingPosition
                                   , CheckingString.substr(branching.StartingPosition
                                                           , branching.CurrentPosition - branching.StartingPosition)));
        //поиск всех возможных переходов из текущего автомата
        auto ptrFirst = SwitchingFunction.lower_bound(std::make_pair(branching.CurrentCondition
                , CheckingString[branching.CurrentPosition]));
        auto ptrLast = SwitchingFunction.upper_bound(std::make_pair(branching.CurrentCondition
                , CheckingString[branching.CurrentPosition]));

        //выполнение копирования автомата во все возможные разветвления
        if (ptrFirst != SwitchingFunction.end()
            && branching.CurrentPosition < CheckingString.size())
                std::for_each(ptrFirst, ptrLast, [&branching, this](const std::pair<std::pair<Condition, Symbol>, Condition> & condition){
                    auto copy = ExactlyAutomatoBranching(branching.StartingPosition ,branching.CurrentPosition + 1, condition.second);
                    this->NonDeterminedAutomatosList.push(copy);
                });
   }
   //добавление всех начальных ветвей в каждый символ строки, чтобы перебрать все возможные подстроки
    auto AddAutomato(Position pos){
        NonDeterminedAutomatosList.push(ExactlyAutomatoBranching(pos, pos, StartingCondition));
    }
    friend class ExactlyAutomatoBranching;

public:
    //конструктор класса
    NonDeterminedAutomato(
            const std::multimap<std::pair<Condition, Symbol>, Condition>& _SwitchingFunction
            ,const Condition& _StartingCondition
            ,const std::list<Condition>& _FinishingConditions
    ):SwitchingFunction(_SwitchingFunction)
            ,StartingCondition(_StartingCondition)
            ,FinishingConditions(_FinishingConditions){}

    //запуск поиска подходящих подстрок
    auto Check(const std::string& CheckingString) -> std::list<std::pair<Position, std::string>>{
        for (int i = 0; i < CheckingString.size(); ++i){
            AddAutomato(i);
        }
        while (!NonDeterminedAutomatosList.empty())
            Step(CheckingString);
        auto answer = std::move(AppropriateSubStrings);
        return answer;
    }
};

int main() {
    //объявление функции перехода
    auto switchingFunction = std::multimap<std::pair<Condition, Symbol>, Condition>{
            std::make_pair(std::make_pair(1, '%'), 2),
            std::make_pair(std::make_pair(2, '+'), 3),
            std::make_pair(std::make_pair(3, 'x'), 4),
            std::make_pair(std::make_pair(4, '+'), 5),
            std::make_pair(std::make_pair(5, '?'), 6),
            std::make_pair(std::make_pair(5, '+'), 3),
            std::make_pair(std::make_pair(5, '%'), 7),
            std::make_pair(std::make_pair(6, '?'), 5),
    };
    //объявление начального состояния автомата
    auto startingCondition = 1;
    //объявление множества конечных состояний
    auto finishingConditions = std::list<Condition>{7};
    //создание автомата по требуемым параметрам
    NonDeterminedAutomato automato(switchingFunction, startingCondition, finishingConditions);

    //ввод строки, в которой будет осуществляться поиск
    auto str = std::string();
    std::cin >> str;

    //поиск всех допустимых подстрок
    auto records = automato.Check(str);
    //вывод результата на экран
    std::for_each(records.begin(), records.end(), [](const std::pair<Position, std::string>& valuePair){
        std::cout << valuePair.first << ", " << valuePair.second << std::endl;
    });
    if (records.size() == 0)
        std::cout << "Nothing" << std::endl;
    else
        std::cout << "That is all" << std::endl;
    return 0;
}
