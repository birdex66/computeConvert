#include <iostream>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <cmath>
#include <vector>

using namespace std;

string octal(string choiceCur, string input);
string hex(string choiceCur,string input);
string decimal(string choiceCur,string input);
string binary(string choiceCur,string input, size_t bitlen);
bool check(string choiceConvert);
bool checkType(string input,string choice);
bool convert();
bool calculate();
bool getHistory();

string oneComplement(string input,string choice);
string add(string inputOne, string inputTwo, string choice);
string twoComplement(string input, string numType);
string subtract(string inputOne,string inputTwo,string numType);

static bool preserve = false;

struct logg{
    string operation;
    string type;

    logg() : operation(""),type("") {}
    logg(string o,string t) : operation(o),type(t) {}
};

static vector<logg> history;

int main(int argc, char** argv){

    while(true){
        string choice;
        if(!preserve) system("clear");

        if(!history.empty()) cout << history[history.size()-1].operation << endl;

        printf("\nWelcome...\n====================\nCalculate\t(1)\nConvert\t\t(2)\nLog\t\t(3)\n\nExit\t\t(E)\n\nEnter Choice:\t");

        getline(cin, choice);
        //if(!preserve) system("clear");
        preserve = false;
        if(choice == "1"){
            if(!calculate()) preserve = true;
        }else if(choice == "2"){
            if(!convert()) preserve = true;
        }else if(choice == "3"){
            if(getHistory()) preserve = true;
        }else if(choice == "E"){
            printf("\n+++++++++++\nExiting...\n");
            break;
        }else{
            printf("Invalid choice. Please try again.\n\n");
            sleep(1);
        }
    }

    return 0;
}

string octal(string choiceCur, string input){
    if(!checkType(input, choiceCur)) return "ERR";

    string convert = "";

    if(choiceCur == "O") return input;
    if(choiceCur != "D") input = decimal(choiceCur,input);

    //cout << "Decimal Val: " << input << endl;

    try{
        long long dec = stoll(input);
        if (dec == 0) return "0";

        for(long long con=dec; con>0; con/=8){
            //convert += to_string(con % 8);
            int check = con % 8;
            convert = to_string(check)+ convert;
        }

    }catch(const std::out_of_range& e){
        cerr << "Error: 64 bit integers only!" << endl;
    }catch(const std::invalid_argument& e){
        cerr << "Error: Wrong data type!" << endl;
    }

    return convert;


}

string hex(string choiceCur,string input){
    if(!checkType(input, choiceCur)) return "ERR";


    string convert = "";

    if(choiceCur == "H") return input;
    if(choiceCur != "D") input = decimal(choiceCur,input);

    //cout << "Decimal Val: " << input << endl;

    try{
        long long dec = stoll(input);
        if(dec == 0) return "0";

        for(long long con = dec; con>0; con/=16){
            //convert += to_string(con % 16);
            int check = con % 16;
            convert = (check > 9 ? char((check - 10) + 'A') : char(check + '0')) + convert;
        }

        //if(!checkType(convert, "H")) return "ERR";

    }catch(const std::out_of_range& e){
        cerr << "Error: 64 bit integers only!" << endl;
    }catch(const std::invalid_argument& e){
        cerr << "Error: Wrong data type!" << endl;
    }

    return convert;
}

string decimal(string choiceCur,string input){
    if(!checkType(input, choiceCur)) return "ERR";

    string convert = "";
    int constant;

    if(choiceCur == "B"){
        constant = 2;
    }else if(choiceCur == "D"){
        return input;
    }else if(choiceCur == "H"){
        constant = 16;
    }else if(choiceCur == "O"){
        constant = 8;
    }

    try{
        long long dec = 0;
        int len = input.length();

        for(int con = 0; con < len; ++con){
            //cout << input[input.length() - 1 - con] << " * 2^" << con << endl;
            char ref = input[len-1-con];

            if(constant == 2){
                dec += (ref == '1') ? pow(constant,con): 0;
            }else if(constant == 8){
                dec += (ref - '0') * pow(constant,con);
            }else if(constant == 16){
                if(ref >= 'A' && ref <= 'F'){
                    dec += (ref - 'A' + 10) * pow(constant,con);
                }else{
                    dec += (ref - '0') * pow(constant, con);
                }

            }
        }
        convert = to_string(dec);

    }catch(const std::out_of_range& e){
        cerr << "Error: 64 bit integers only!" << endl;
    }catch(const std::invalid_argument& e){
        cerr << "Error: Wrong data type!" << endl;
    }

    return convert;
}

string binary(string choiceCur,string input, size_t bitlen){
    if(!checkType(input, choiceCur)) return "ERR";

    string convert = "";
    size_t bitref = bitlen;

    try{
        if(choiceCur == "B") return input;
        if(choiceCur != "D") input = decimal(choiceCur,input);

        long long dec = stoll(input);

        if (dec == 0 && bitlen == 0){
            convert = "0";
        }else if(bitlen == 0){
            for(long long con=dec; con>0; con/=2){
                string ref = to_string(con % 2);
                if(ref != "1" && ref != "0") return "Error\n";
                convert = ref + convert;
            }
        }else{
            while(bitref > 0){
                int ref = 1 << (bitref-1);
                if(ref <= dec){
                    convert += "1";
                    dec -= ref;
                }else{
                    convert += "0";
                }
                bitref--;
            }

            string check = binary(choiceCur,input,0);
            if(stoll(convert) < stoll(check)){
                printf("\nError: Input exceeds bit buffer. Returning lowest available estimate...\n");
                return check;
            }
        }

        if(convert.size() < bitlen){
            convert = string(bitlen - convert.size(),'0') + convert;
        }


    }catch(const std::out_of_range& e){
        cerr << "Error: 64 bit integers max!" << endl;
    }catch(const std::invalid_argument& e){
        cerr << "Error: Wrong data type!" << endl;
    }

    return convert;
}


bool convert(){
    string choiceConvert;
    string choiceCur;
    string input;

    printf("\nConvert your text to:\n--------------------\nBinary\t\t(B)\nDecimal\t\t(D)\nHexaDecimal\t(H)\nOctal\t\t(O)\n\nBack\t\t(E)\n\nEnter Choice: ");
    getline(cin, choiceConvert);

    if(!check(choiceConvert)) return false;

    if(choiceConvert == "E"){
        printf("\n\n");
        return false;
    }


    printf("What text are you converting from? (B,D,H,O): ");
    getline(cin, choiceCur);

    if(!check(choiceCur)) return false;

    printf("Input -> ");
    getline(cin, input);


    string out;


    if((choiceConvert == "B")){
        try{
            string size;

            printf("Specify the  maximum bit length(default is min): ");
            getline(cin, size);
            size_t bitlen;

            if(size == "") bitlen = 0;
            else bitlen = static_cast<size_t>(stoll(size));

            out = binary(choiceCur,input,bitlen);
        }catch(const std::invalid_argument& e){ cerr << "Wrong data type!\n" << endl;}
    }else if(choiceConvert == "D") out = decimal(choiceCur,input);
     else if(choiceConvert == "H") out = hex(choiceCur,input);
     else if(choiceConvert == "O") out = octal(choiceCur,input);
     else{
        printf("\n\n");
        return false;
     }


    if((out == "") || (out == "ERR")) return false;
    string fer = "\n=============\n" + input + " >> " + out  + " (" + to_string(out.length()) + " bits)\n=============\n";
    logg gol(fer,"convert");
    history.push_back(gol);
    cout << fer << endl;
    cout << "Press ENTER to continue... ";
    getchar();
    return true;
}

bool calculate(){
    string choice;

    printf("\n===========\nSelect your operation...\n\nOne's Complement (O)\nAddition\t (A)\nTwo's Complement (C)\nSubtraction\t (S)\n\n\nBack\t\t (E)\n\nEnter Choice:\t");
    getline(cin, choice);

    string out;
    string fer;

    if(choice == "O"){
        printf("What text are you converting from? (B,D,H,O): ");
        getline(cin, choice);
        if(!check(choice)) return false;

        string input;
        printf("Input -> ");
        getline(cin, input);


        out = oneComplement(input,choice);

        //if((out == "") || (out == "ERR")) return;

        //cout << "\n=============\n" << input << " >> " << out <<"\n=============\n" << endl;
        fer = "\n=============\n" + input + " >> " + out + "\n=============\n" ;
    }else if(choice == "A"){
        string inputOne;
        string inputTwo;
        string numType;

        printf("What text are you converting from? (B,D,H,O): ");
        getline(cin, numType);
        if(!check(numType)) return false;

        printf("Input One >> ");
        getline(cin, inputOne);
        printf("Input Two >> ");
        getline(cin, inputTwo);

        out = add(inputOne, inputTwo, numType);

        //if((out == "") || (out == "ERR")) return;

        //cout << "\n=============\n" << inputOne << " + " << inputTwo << " = " << out << " ("<< out.size() << " bits)\n=============\n" << endl;
        fer = "\n=============\n" + inputOne + " + " + inputTwo + " = " + out + " (" + to_string(out.size()) + " bits)\n=============\n";
    }else if(choice == "C"){
        string input;
        string numType;

        printf("What text are you converting from? (B,D,H,O): ");
        getline(cin, numType);
        if(!check(numType)) return false;

        printf("Input >> ");
        getline(cin, input);

        out = twoComplement(input, numType);

        //if((out == "") || (out == "ERR")) return;

        //cout << "\n=============\n" << input << " >> " << out <<"\n=============\n" << endl;
        fer = "\n=============\n" + input + " >> " + out + "\n=============\n"; 
    }else if(choice == "S"){
        string inputOne;
        string inputTwo;
        string numType;

        printf("What text are you converting from? (B,D,H,O): ");
        getline(cin, numType);
        if(!check(numType)) return false;

        printf("Input One >> ");
        getline(cin, inputOne);
        printf("Input Two >> ");
        getline(cin, inputTwo);
        out = subtract(inputOne, inputTwo, numType);

        //if((out == "") || (out == "ERR")) return;

        //cout << "\n=============\n" << inputOne << " - " << inputTwo << " = " << out << " ("<< out.size() << " bits)\n=============\n" << endl;
        fer = "\n=============\n" + inputOne + " - " + inputTwo + " = " + out + " (" + to_string(out.size()) + " bits)\n=============\n";
        
    }else{
        printf("\n\n");
        return false;
    }

    if((out == "") || (out == "ERR")) return false;
    cout << fer << endl;
    logg gol(fer,"calculate");
    history.push_back(gol);
    cout << "Press ENTER to continue... ";
    getchar();
    return true;
}


string twoComplement(string input, string numType){
    if(!checkType(input,numType)) return "ERR";

    if(numType != "B"){
        input = binary(numType,input,0);
    }

    input = oneComplement(input,"B");

    return add(input,"1","B");
}

string subtract(string inputOne, string inputTwo, string numType) {
    if(!checkType(inputOne,numType) || !checkType(inputTwo,numType)) return "ERR";

    if (numType != "B") {
        if(stoll(inputOne) > stoll(inputTwo)){
            inputOne = binary(numType, inputOne,0);
            inputTwo = binary(numType, inputTwo,inputOne.length());
        }else{
            inputTwo = binary(numType, inputTwo,0);
            inputOne = binary(numType, inputOne,inputTwo.length());
        }
    }

    size_t bitLength = max(inputOne.size(), inputTwo.size());
    //inputOne = string(bitLength - inputOne.size(), '0') + inputOne;
    //inputTwo = string(bitLength - inputTwo.size(), '0') + inputTwo;

    //cout << "\n****\n\t" << inputOne << "\n-\t" << inputTwo << "\n___________\n" << endl;
    inputTwo = twoComplement(inputTwo, "B");

    //cout << "\n****\n\t" << inputOne << "\n-\t" << inputTwo << "\n___________\n" << endl;

    string result = add(inputOne, inputTwo, "B");

    size_t outputLength = max(result.size(), bitLength);
    //result = string(outputLength - result.size(), '0') + result;
    result = result.substr(1, outputLength);

    //cout << "Result (pre-two's complement): " << result << endl;
    //cout << "Final Result: " << result << endl;

    // Convert to the requested number format
    if (numType == "D") {
        result = decimal("B", result);
    } else if (numType == "H") {
        result = hex("B", result);
    } else if (numType == "O") {
        result = octal("B", result);
    }

    return result;
}


string add(string inputOne,string inputTwo,string numType){
    if(!checkType(inputOne,numType) || !checkType(inputTwo,numType)) return "ERR";

    if(numType != "B"){
        inputOne = binary(numType,inputOne,0);
        inputTwo = binary(numType,inputTwo,0);
    }

    string out = "";
    int carryin = 0;
    size_t maxLen = max(inputOne.size(), inputTwo.size());


    reverse(inputOne.begin(), inputOne.end());
    reverse(inputTwo.begin(), inputTwo.end());

    for(size_t i = 0; i < maxLen; ++i){
        int inOneRef = inputOne[i];
        int inTwoRef = inputTwo[i];

        int ref1 = (i < inputOne.length()) ? inOneRef - '0' : 0;
        int ref2 = (i < inputTwo.length()) ? inTwoRef - '0' : 0;
        int sum = (ref1 ^ ref2) ^ carryin;
        out = (char)(sum + '0') + out;

        carryin = (ref1 & ref2) | (carryin & (ref1 ^ ref2));

    }

    if(carryin) out = (char)(carryin + '0') + out;

    if(numType == "D"){
        out =  decimal("B",out);
    }else if(numType == "H"){
        out = hex("B",out);
    }else if(numType == "O"){
        out = octal("B",out);
    }


    return out;
}


string oneComplement(string input,string choice){
    if(!checkType(input,choice)) return "ERR";

    if(choice != "B") input = binary(choice,input,0);

    string out = "";

    for(char c : input){
        if(c != '0' && c != '1') return "Error: Input must be binary (0 or 1)";
        out += (c - '0') == 0 ? '1' : '0';
    }

    if(choice == "D"){
        return decimal("B",out);
    }else if(choice == "H"){
        return hex("B",out);
    }else if(choice == "O"){
        return octal("B",out);
    }else{
        return out;
    }
}



bool check(string choiceConvert){
    //if(choiceConvert == "ERR") return false;

    if(!(choiceConvert =="B" || choiceConvert == "D" || choiceConvert == "H" || choiceConvert == "O" || choiceConvert == "E")){
        printf("Invalid choice. Please try again.\n\n");
        sleep(1);
        return false;
    }else{
        return true;
    }
}


bool checkType(string input,string choice){
    bool ret = true;
    if(choice == "B"){
        for(char c : input) if(c != '0' && c != '1') ret =  false;
    }else if(choice == "D"){
        for(char c : input) if (c < '0' || c > '9') ret = false;
    }else if(choice == "H"){
        for(char c : input) if(!((c>='0'&& c<='9')||(c>='A' && c<='F'))) ret = false;
    }else if(choice == "O"){
        for(char c : input) if (c < '0' || c > '7') ret = false;
    }else{
        cout << "Error: Invalid choice\n";
        return false;
    }

    if(!ret) cout << "Error: Wrong data type!" << endl;
    return ret;
}

bool getHistory(){
    string choice;
    
    if(history.empty()){
        cout << "\n\nList is empty. Press any key to continue." << endl;
        getchar();
        return false;
    }
    cout << "\nFilter?\n====================\n\nHistory\t\t(1)\nCalculations\t(2)\nConversions\t(3)\n\nBack\t\t(B)\n\nEnter Choice:\t";
    getline(cin, choice);
    system("clear");
    if(choice == "1") for(logg l : history) cout << l.operation << endl;
    else if(choice == "2"){ for(logg l : history) if(l.type == "calculate") cout << l.operation << endl;}
    else if(choice == "3") { for(logg l : history) if(l.type == "convert") cout << l.operation << endl;}
    return true;
}
