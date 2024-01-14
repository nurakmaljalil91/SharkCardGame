//
// Created by User on 14/1/2024.
//

#ifndef CARD_H
#define CARD_H
#include <string>


class Card {
public:
    Card(int value, std::string suit);

    ~Card();

    [[nodiscard]] std::string getName() const;

private:
    int mValue = 0;
    std::string mSuit;
};


#endif //CARD_H
