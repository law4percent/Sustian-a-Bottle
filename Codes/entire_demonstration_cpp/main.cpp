#include "iostream"
#include "unistd.h"
using namespace std;

int Converter(int numberOfbottles);
int CountCoin(int limit);
int HoldTheCoin(int getSum);
void DisplayQnty(int Qnty);

int main()
{
    char getBTL;
    int countBTL = 0;
    int sumOfCoin = 0;
    int DisplayCoinSum = 0;
    int Time = 0;
    system("clear");
    DisplayQnty(0);
    
    do
    {
        cout << "\nInsert the bottle:\n"
                "Type b or B\n";
        cout << ">> ";
        cin >> getBTL;
        system("clear");

        if (getBTL == 'b')
        {
            countBTL += 1;
            DisplayQnty(1);
        }
        else if (getBTL == 'B')
        {
            countBTL += 2;
            DisplayQnty(2);
        }
        else
        {
            DisplayQnty(0);
            cout << "Not a bottle." << endl;
        }

        if (countBTL == 3)
        {
            sumOfCoin += CountCoin( Converter(countBTL) );
            DisplayCoinSum = HoldTheCoin(sumOfCoin);

            sumOfCoin = 0;
            countBTL = 0;
        }
        else if (countBTL == 4)
        {
            sumOfCoin += CountCoin( Converter(countBTL-1) );
            DisplayCoinSum = HoldTheCoin(sumOfCoin);

            sumOfCoin = 0;
            countBTL = 1;
        }

        Time = 30 * DisplayCoinSum;
        cout << "\nCoin: " << DisplayCoinSum << " Peso is equivalent to ";
        if (Time < 60)
            cout << Time << " min/s";
        else if (DisplayCoinSum % 2 != 0)
            cout << Time / 60 <<" hr/s and 30 min/s";
        else
            cout << Time / 60 <<" hr/s";
        cout << " Internet.\n\n";

        sleep(2);
    } while (true);

    return 0;
}

int HoldTheCoin(int getSum)
{
    static int HoldSum = 0;

    HoldSum += getSum;
    cout << "Update timer.\n" << endl;
    return HoldSum;
}

int Converter(int numberOfbottles)
{
    if (numberOfbottles >= 3)
        return numberOfbottles /= 3;
    else
        return 0;
}

int CountCoin(int limit)
{
    int sum = 0, i;

    for (i = 1; i <= limit; i++)
        sum += 1;

    return sum;
}

void DisplayQnty(int Qnty)
{
    static int qnty = 0;

    qnty += Qnty;
   cout << "\nQuantity: " << qnty << endl;
}