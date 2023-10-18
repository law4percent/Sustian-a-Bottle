#include "iostream"
using namespace std;

int Converter(int numberOfbottles);
int CountCoin(int limit);
int HoldTheCoin(int getSum);

int main()
{
    char getBTL;
    int countBTL = 0;
    int CNT = 0;
    int sumOfCoin = 0;

    while (true)
    {
        cout << "Qnty: " << countBTL << endl;
        do
        {
            cout << "Insert the bottle: ";
            cin >> getBTL;

            if (getBTL == 'c')
            {
                CNT = Converter(countBTL);
                sumOfCoin += CountCoin(CNT);

                cout << "Coin: " << HoldTheCoin(sumOfCoin) << " Peso" << endl << endl;

                // RESET
                sumOfCoin = 0;
                countBTL = 0;
                CNT = 0;
            }
            else if (getBTL == 'b')
                countBTL += 1;
            else if (getBTL == 'B')
                countBTL += 2;
            else
                cout << "Not a bottle." << endl;

            cout << "Qnty: " << countBTL << endl;
        } while (true);
    }

}

int HoldTheCoin(int getSum)
{
    static int HoldSum = 0;

    HoldSum += getSum;
    return HoldSum;
}

int Converter(int numberOfbottles)
{
    if (numberOfbottles >= 3)
        return numberOfbottles /= 3;
    else return 0;
}

int CountCoin(int limit)
{
    int sum = 0, i;

    for (i = 1; i <= limit; i++)
        sum += 1;

    return sum;
}