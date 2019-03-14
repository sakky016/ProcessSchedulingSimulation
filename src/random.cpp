#include "random.h"

/*******************************************************************************************************************/
RandomGenerator::RandomGenerator()
{
    m_bIsSeedGenerated = false;
}

/*******************************************************************************************************************/
RandomGenerator::~RandomGenerator()
{

}

/*******************************************************************************************************************/
void RandomGenerator::generateSeed()
{
    srand((unsigned)time(0));
    m_bIsSeedGenerated = true;
}

/*******************************************************************************************************************/
/* Generate a random number in between [0, range) */
/*******************************************************************************************************************/
unsigned int RandomGenerator::generateRandomNumber(unsigned int range)
{
    //unsigned int actualRange = range + 1;

    if (m_bIsSeedGenerated)
    {
        return rand() % range;
    }
    else
    {
        generateSeed();
        return rand() % range;
    }
}

/*******************************************************************************************************************/
/* @param len:                 Length of string to generate.
@param useUppercase         Generate uppercase characters
@param useNumbers           Generate numbers
@returns                    Pointer to a string present in heap memory, user has responsibility
to free the memory used by this string.
********************************************************************************************************************/
char *RandomGenerator::generateRandomString(int len, bool useUppercase, bool useNumbers)
{
    bool firstShouldBeChar = true;
    const int UPPER_CASE_START = 65; //A
    const int UPPER_CASE_END = 90;  //Z
    const int LOWER_CASE_START = 97;  //a
    const int LOWER_CASE_END = 122;  //z
    const int NUMBER_START = 48;  //0
    const int NUMBER_END = 57;  //9
    const int CHARS = 26;
    const int NUMBERS = 10;

    int strLen = len + 1;
    char *output = (char *)malloc(strLen * sizeof(char));
    memset(output, 0, strLen);

    int i = 0;
    int val = 0;
    unsigned int valId = 0;

    /* Generate 1st as a lower case character */
    if (firstShouldBeChar && i == 0)
    {
        val = LOWER_CASE_START + generateRandomNumber(CHARS);
        output[i] = val;
        i++;
    }

    while (i < len)
    {
        valId = generateRandomNumber(90);

        if (useUppercase && valId < 30)
        {
            //Generate uppercase
            val = UPPER_CASE_START + generateRandomNumber(CHARS);
        }
        else if (useNumbers && valId < 60)
        {
            //Generate number
            val = NUMBER_START + generateRandomNumber(NUMBERS);
        }
        else
        {
            // Genereate lowercase
            val = LOWER_CASE_START + generateRandomNumber(CHARS);
        }

        output[i] = val;
        i++;
    }

    output[i] = '\0';

    return output;
}