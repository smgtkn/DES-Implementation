


#include "utils.h"
#include <time.h>

#define MAX_PLAIN_LENGTH  64

void myXor(int size, char *inp1, char *inp2, char *result) {
    char temp[size + 1];
    for (int i = 0; i < size; i++) {
        if (inp1[i] == inp2[i]) {
            temp[i] = '0';
        } else {
            temp[i] = '1';
        }
    }
    temp[size] = '\0';

    for (int i = 0; i < size; i++) {
        result[i] = temp[i];
    }
    result[size] = '\0';
}

void computePermutation(int *permutationMap, char *toBePermuted, int permutationLength, char *result) {
    char temp[permutationLength];

    for (int i = 0; i < permutationLength; i++) {
        temp[i] = toBePermuted[permutationMap[i] - 1];
    }
    for (int i = 0; i < permutationLength; i++) {
        result[i] = temp[i];
    }
}

void shiftKey(int roundNumber, char *binaryKey_56) {
    char temp[56];
    int shiftAmount = roundShifts[roundNumber];
    for (int i = 0; i < 28; i++) {
        temp[i] = binaryKey_56[(i + shiftAmount) % 28];
    }
    for (int i = 28; i < 56; i++) {
        int index = (i + shiftAmount) % 56;
        if (index < 28) {
            index = index + 28;
        }
        temp[i] = binaryKey_56[index];
    }
    for (int i = 0; i < 56; i++) {
        binaryKey_56[i] = temp[i];
    }
    binaryKey_56[56] = '\0';

}

void padTheHexaString(char *hexaPlainText) {
    int hexaLength = strlen(hexaPlainText);

    int padSize = 64 - hexaLength % 64;

    hexaPlainText[hexaLength] = '1';
    int i = 0;
    for (i = 1; i < padSize; i++) {
        hexaPlainText[i + hexaLength] = '0';
    }
    hexaPlainText[i + hexaLength] = '\0';

}

void doSboxes(char *input, char *output) {
    char temp[5];
    memset(temp, '\0', sizeof(temp));

    for (int i = 0; i < 8; i++) {

        if (input[i * 6] == '0' && input[i * 6 + 5] == '0') {
            strncpy(temp, input + i * 6 + 1, 4);
            strncpy(output + i * 4, convertToBin[sboxes[i][0 + strtol(temp, NULL, 2)]], 4);

        } else if (input[i * 6] == '0' && input[i * 6 + 5] == '1') {
            strncpy(temp, input + i * 6 + 1, 4);
            strncpy(output + i * 4, convertToBin[sboxes[i][16 + strtol(temp, NULL, 2)]], 4);


        } else if (input[i * 6] == '1' && input[i * 6 + 5] == '0') {
            strncpy(temp, input + i * 6 + 1, 4);
            strncpy(output + i * 4, convertToBin[sboxes[i][32 + strtol(temp, NULL, 2)]], 4);
        } else if (input[i * 6] == '1' && input[i * 6 + 5] == '1') {
            strncpy(temp, input + i * 6 + 1, 4);
            strncpy(output + i * 4, convertToBin[sboxes[i][48 + strtol(temp, NULL, 2)]], 4);
        }
    }
}

void swapLeftRight(char *left, char *right) {
    char temp[32];
    for (int i = 0; i < 32; i++) {
        temp[i] = left[i];
        left[i] = right[i];
        right[i] = temp[i];
    }

}

void fillLeftRight(char *singleBlock, char *left, char *right) {
    for (int i = 0; i < 32; i++) {
        left[i] = singleBlock[i];
    }
    for (int i = 32; i < 64; i++) {
        right[i - 32] = singleBlock[i];
    }
}

void computeKeys(char *key, char roundKeys[16][49]) {

    char binaryKey[65];
    char binaryKey_56[57];
    char binaryKey48inRounds[48];
    memset(binaryKey, '\0', sizeof(binaryKey));
    memset(binaryKey_56, '\0', sizeof(binaryKey_56));
    memset(binaryKey48inRounds, '\0', sizeof(binaryKey48inRounds));

    //convert key to binary string on var binaryKey
    hexToBin(key, binaryKey);


    // key permutation 1, output is on binaryKey_56
    computePermutation(keyPermutation1, binaryKey, 56, binaryKey_56);
    for (int i = 0; i < 16; i++) {
        shiftKey(i, binaryKey_56);

        computePermutation(keyPermutation2, binaryKey_56, 48, roundKeys[i]);
        //  printf("key %s \n",roundKeys[i]);
        //  binToHex(roundKeys[i],NULL);
    }
}

void DES(char *singleBlock, char *key, char *output, char roundKeys[16][49]) {


    char binaryKey[65];
    char binaryKey_56[57];
    char binaryKey48inRounds[48];

    char extendedBlock[49];
    char left[33];
    char right[33];
    char afterSbox[33];

    memset(binaryKey, '\0', sizeof(binaryKey));
    memset(binaryKey_56, '\0', sizeof(binaryKey_56));
    memset(binaryKey48inRounds, '\0', sizeof(binaryKey48inRounds));
    memset(extendedBlock, '\0', sizeof(extendedBlock));
    memset(left, '\0', sizeof(left));
    memset(right, '\0', sizeof(right));
    memset(afterSbox, '\0', sizeof(afterSbox));


    //initial permutation
    computePermutation(initialPermutation, singleBlock, 64, singleBlock);



    //find left right
    fillLeftRight(singleBlock, left, right);

    for (int i = 0; i < 16; i++) {


        //extend right
        computePermutation(expansionFunction, right, 48, extendedBlock);

        //xor extended right with round key
        myXor(48, roundKeys[i], extendedBlock, extendedBlock);

        //sbox
        doSboxes(extendedBlock, afterSbox);

        //permutation after xbox
        computePermutation(permAfterSbox, afterSbox, 32, afterSbox);

        myXor(32, afterSbox, left, left);


        if (i != 15) {
            swapLeftRight(left, right);
            // printf("\nleft "); binToHex(left); printf(" right "); binToHex( right); printf("\n");
        }
        //   printf("\n round %d . key:  ", i + 1);
        //   binToHex(binaryKey48inRounds,NULL);
        //   printf(" round output: ");
        //   binToHex(left,NULL);
        //    binToHex(right,NULL);
        //    printf("\n");
    }
    for (int i = 0; i < 32; i++) {

        singleBlock[i] = left[i];
    }
    for (int i = 32; i < 64; i++) {
        singleBlock[i] = right[i - 32];
    }
    computePermutation(finalPermutation, singleBlock, 64, singleBlock);

    binToHex(singleBlock, output);


}

int main() {
    int size;
    char plaintext[MAX_PLAIN_LENGTH + 1];
    char binaryPlainText[MAX_PLAIN_LENGTH * 8 + 1];
    char hexaPlainText[MAX_PLAIN_LENGTH * 2 + 1];

    char *key = "0123456789abcdef\0";
    char singleBlock[65];
    char output[9];
    char *initVector = "0123456789abcdef\0";
    char initVectorBin[65];

    char roundKeys[16][49];
    for (int i = 0; i < 16; i++) {
        memset(roundKeys[i], '\0', sizeof(roundKeys[i]));
    }

    memset(singleBlock, '\0', sizeof(singleBlock));
    memset(binaryPlainText, '\0', sizeof(binaryPlainText));
    memset(hexaPlainText, '\0', sizeof(hexaPlainText));
    memset(plaintext, '\0', sizeof(plaintext));
    memset(output, '\0', sizeof(output));
    memset(initVectorBin, '\0', sizeof(initVectorBin));

    hexToBin(initVector, initVectorBin);

    fgets(plaintext, MAX_PLAIN_LENGTH, stdin);
    plaintext[strlen(plaintext) - 1] = '\0';
    decToBin(binaryPlainText, plaintext);
    padTheHexaString(binaryPlainText);

    int numOfBlocks = strlen(binaryPlainText) / 64;

    binToHex(binaryPlainText, hexaPlainText);
    printf("Plaintext: %s\nPadded hexadecimal text: %s\n", plaintext, hexaPlainText);


    //Start the timer

    clock_t begin = clock();

    //Compute the round keys

    computeKeys(key, roundKeys);
    printf("Ciphertext: ");
   // unsigned long computeTime = pow(2, 23);
   // for (unsigned long k = 0; k < computeTime; k++) {
        for (int i = 0; i < numOfBlocks; i++) {
            strncpy(singleBlock, binaryPlainText + 64 * i, 64);

            myXor(64, initVectorBin, singleBlock, singleBlock);

            DES(singleBlock, key, output, roundKeys);
            printf("%s", output);
            strncpy(initVectorBin, singleBlock, 64);

        }
   // }
    printf("\n");




    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("Time: %f\n", time_spent);
    return 0;
}
