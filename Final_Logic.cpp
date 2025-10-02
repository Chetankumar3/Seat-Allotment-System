#include <bits/stdc++.h>
using namespace std;

string ApplicantsFilePath = "./Cleaned Applicants.csv";
string SeatMatrixFilePath = "./new_seat_matrix.csv";
string AllotmentsFilePath = "./allotted.csv";
typedef long long ll;

int mapChoice(const string &s) {
    if (s == "CSE") return 0;
    else if (s == "DSAI") return 1;
    else if (s == "ECE") return 2;
    else return 0;
}

int mapQuota(const string &s){
    if (s == "Chhattisgarh") return 1;
    else if (s == "NTPC") return 2;
    else if (s == "Kashmiri Migrant") return 3;
    else return 0;
}

bool yesNoToBool(const string &s) {
    return (s != "No");
}

class Applicant {
public:
    ll SerialNumber;
    ll ApplicationNumber;
    string Name;
    ll Rank;
    string Category;
    string Quota;
    string Gender;
    string EmailId;
    int Choice1;
    int Choice2;
    int Choice3;

    bool IsOpen;
    bool IsFemale;
    bool IsEXG;
    bool IsPWD;
    bool IsFF;
    bool IsPVTP;

    bool IsUR;
    bool IsSC;
    bool IsST;
    bool IsOBC;
    bool IsEWS;

    int AppliedQuota;
    int AllottedBranch = -1;
    string AllottedSeat;
    string AllottedCategory;
    string AllottedQuota;
    string AllottedSeatLabel;

    Applicant(const vector<string> &f) {
        SerialNumber = stoll(f[0]);
        ApplicationNumber = stoll(f[1]);
        Name = f[2];
        EmailId = f[13];
        string gender = f[3];
        string category = f[4];
        if(category == "OBC NCL" || category == "OBC-NCL") category = "OBC";

        Choice1 = mapChoice(f[5]);
        Choice2 = mapChoice(f[6]);
        Choice3 = mapChoice(f[7]);
        Rank = stoll(f[8]);
        AppliedQuota = mapQuota(f[9]);

        IsOpen = (gender == "Male");
        IsFemale = (gender == "Female");
        Gender = IsOpen ? "M" : "F";

        IsPWD = yesNoToBool(f[10]);
        IsEXG = yesNoToBool(f[11]);
        IsFF = yesNoToBool(f[12]);

        IsPVTP = false;

        IsUR = (category == "UR");
        IsSC = (category == "SC");
        IsST = (category == "ST");
        IsEWS = (category == "EWS");
        IsOBC = (category == "OBC");
        Category = category;
        
        if(IsPWD) Quota = "PWD";
        else if(IsEXG) Quota = "EXG";
        else if(IsFF) Quota = "FF";
        else Quota = "OPEN";

        AllottedCategory = "";
        AllottedSeatLabel = "";
    }

    void DisplayApplicant() {
        cout << "S No.: " << SerialNumber << ", ";
        cout << "Appn No.: " << ApplicationNumber << ", ";
        cout << "Name: " << Name << ", ";
        cout << "EMail-ID: " << EmailId << ", ";
        cout << "Rank: " << Rank << ", ";
        cout << "Preferences: " << Choice1 << " " << Choice2 << " " << Choice3 << ", ";
        cout << "Applied Quota: " << AppliedQuota << ", ";
        cout << "Category: " << Category << ", ";
        cout << "Gender: " << Gender << ", ";
        cout << "Quota: " << Quota << ", ";
        cout << "Allotted Branch: " << AllottedBranch << ", ";
        cout << "Allotted Seat: " << AllottedSeat << "\n";
    }
};

int SeatsFilled = 0, Capacity = 3, Iteration = 1;
vector<Applicant> Applicants;
vector<unordered_map<string, unordered_map<string, multiset<string>>>> CHSeats(3), NTPCSeats(3);
unordered_map<string, unordered_map<string, unordered_map<string, vector<pair<string, string>>>>> LogicLines = {
    { "UR", {
        { "M", {
            { "OPEN", { {"UR", "OPEN"} } },
            { "EXG" , { {"UR", "EXG" }, {"UR", "OPEN"} } },
            { "FF"  , { {"UR", "FF"  }, {"UR", "OPEN"} } },
            { "PWD", { {"UR", "PWD" }, {"UR", "OPEN"} } },
        }},
        { "F", {
            { "OPEN", { {"UR", "F"}, {"UR", "OPEN"} } },
            { "EXG" , { {"UR", "F"}, {"UR", "EXG"}, {"UR", "OPEN"} } },
            { "FF"  , { {"UR", "F"}, {"UR", "FF"}, {"UR", "OPEN"} } },
            { "PWD", { {"UR", "F"}, {"UR", "PWD"}, {"UR", "OPEN"} } },
        }}
    }},
    { "SC", {
        { "M", {
            { "OPEN", { {"UR", "OPEN"}, {"SC", "OPEN"} } },
            { "EXG" , { {"UR", "EXG"}, {"UR", "OPEN"}, {"SC", "EXG"}, {"SC", "OPEN"} } },
            { "FF"  , { {"UR", "FF"}, {"UR", "OPEN"}, {"SC", "FF"}, {"SC", "OPEN"} } },
            { "PWD", { {"UR", "PWD"}, {"UR", "OPEN"}, {"SC", "PWD"}, {"SC", "OPEN"} } },
        }},
        { "F", {
            { "OPEN", { {"UR", "F"}, {"UR", "OPEN"}, {"SC", "F"}, {"SC", "OPEN"} } },
            { "EXG" , { {"UR", "F"}, {"UR", "EXG"}, {"UR", "OPEN"}, {"SC", "F"}, {"SC", "EXG"}, {"SC", "OPEN"} } },
            { "FF"  , { {"UR", "F"}, {"UR", "FF"}, {"UR", "OPEN"}, {"SC", "F"}, {"SC", "FF"}, {"SC", "OPEN"} } },
            { "PWD", { {"UR", "F"}, {"UR", "PWD"}, {"UR", "OPEN"}, {"SC", "F"}, {"SC", "PWD"}, {"SC", "OPEN"} } },
        }}
    }},
    { "ST", {
        { "M", {
            { "OPEN", { {"UR", "OPEN"}, {"ST", "OPEN"} } },
            { "EXG" , { {"UR", "EXG"}, {"UR", "OPEN"}, {"ST", "EXG"}, {"ST", "OPEN"} } },
            { "FF"  , { {"UR", "FF"}, {"UR", "OPEN"}, {"ST", "FF"}, {"ST", "OPEN"} } },
            { "PWD", { {"UR", "PWD"}, {"UR", "OPEN"}, {"ST", "PWD"}, {"ST", "OPEN"} } },
        }},
        { "F", {
            { "OPEN", { {"UR", "F"}, {"UR", "OPEN"}, {"ST", "F"}, {"ST", "OPEN"} } },
            { "EXG" , { {"UR", "F"}, {"UR", "EXG"}, {"UR", "OPEN"}, {"ST", "F"}, {"ST", "EXG"}, {"ST", "OPEN"} } },
            { "FF"  , { {"UR", "F"}, {"UR", "FF"}, {"UR", "OPEN"}, {"ST", "F"}, {"ST", "FF"}, {"ST", "OPEN"} } },
            { "PWD", { {"UR", "F"}, {"UR", "PWD"}, {"UR", "OPEN"}, {"ST", "F"}, {"ST", "PWD"}, {"ST", "OPEN"} } },
        }}
    }},
    { "OBC", {
        { "M", {
            { "OPEN", { {"UR", "OPEN"}, {"OBC", "OPEN"} } },
            { "EXG" , { {"UR", "EXG"}, {"UR", "OPEN"}, {"OBC", "EXG"}, {"OBC", "OPEN"} } },
            { "FF"  , { {"UR", "FF"}, {"UR", "OPEN"}, {"OBC", "FF"}, {"OBC", "OPEN"} } },
            { "PWD", { {"UR", "PWD"}, {"UR", "OPEN"}, {"OBC", "PWD"}, {"OBC", "OPEN"} } },
        }},
        { "F", {
            { "OPEN", { {"UR", "F"}, {"UR", "OPEN"}, {"OBC", "F"}, {"OBC", "OPEN"} } },
            { "EXG" , { {"UR", "F"}, {"UR", "EXG"}, {"UR", "OPEN"}, {"OBC", "F"}, {"OBC", "EXG"}, {"OBC", "OPEN"} } },
            { "FF"  , { {"UR", "F"}, {"UR", "FF"}, {"UR", "OPEN"}, {"OBC", "F"}, {"OBC", "FF"}, {"OBC", "OPEN"} } },
            { "PWD", { {"UR", "F"}, {"UR", "PWD"}, {"UR", "OPEN"}, {"OBC", "F"}, {"OBC", "PWD"}, {"OBC", "OPEN"} } },
        }}
    }}
};
unordered_map<string, unordered_map<string, unordered_map<string, vector<pair<string, string>>>>> NTPCLogicLines = {
    { "UR", {
        {"M", {
            {"OPEN", { {"UR","OPEN"} }},
            {"PWD",  { {"UR","PWD"}, {"UR","OPEN"} }}
        }},
        {"F", {
            {"OPEN", { {"UR","F"}, {"UR","OPEN"} }},
            {"PWD",  { {"UR","F"}, {"UR","PWD"}, {"UR","OPEN"} }}
        }}
    }},
    { "SC", {
        {"M", {
            {"OPEN", { {"SC","OPEN"} }},
            {"PWD",  { {"SC","PWD"}, {"SC","OPEN"} }}
        }},
        {"F", {
            {"OPEN", { {"SC","F"}, {"SC","OPEN"} }},
            {"PWD",  { {"SC","F"}, {"SC","PWD"}, {"SC","OPEN"} }}
        }}
    }},
    { "ST", {
        {"M", {
            {"OPEN", { {"ST","OPEN"} }},
            {"PWD",  { {"ST","PWD"}, {"ST","OPEN"} }}
        }},
        {"F", {
            {"OPEN", { {"ST","F"}, {"ST","OPEN"} }},
            {"PWD",  { {"ST","F"}, {"ST","PWD"}, {"ST","OPEN"} }}
        }}
    }},
    { "OBC", {
        {"M", {
            {"OPEN", { {"OBC","OPEN"} }},
            {"PWD",  { {"OBC","PWD"}, {"OBC","OPEN"} }}
        }},
        {"F", {
            {"OPEN", { {"OBC","F"}, {"OBC","OPEN"} }},
            {"PWD",  { {"OBC","F"}, {"OBC","PWD"}, {"OBC","OPEN"} }}
        }}
    }},
    { "EWS", {
        {"M", {
            {"OPEN", { {"EWS","OPEN"} }},
            {"PWD",  { {"EWS","PWD"}, {"EWS","OPEN"} }}
        }},
        {"F", {
            {"OPEN", { {"EWS","F"}, {"EWS","OPEN"} }},
            {"PWD",  { {"EWS","F"}, {"EWS","PWD"}, {"EWS","OPEN"} }}
        }}
    }}
};
unordered_map<string, int> KashmiriSeats = { {"CSE", 1}, {"DSAI", 1}, {"ECE", 1} };
vector<vector<string>> MergedAllotments;
map<int, string> IntToBranch = { {0, "CSE"}, {1, "DSAI"}, {2, "ECE"} };

vector<string> splitCSV(const string &line) {
    vector<string> out;
    string cur;
    stringstream ss(line);
    while (getline(ss, cur, ',')) {
        out.push_back(cur);
    }
    return out;
}

bool ExtractingDataFromCSV() {
    ifstream fin(ApplicantsFilePath);
    if (!fin.is_open()) {
        cerr << "Error opening: " << ApplicantsFilePath << "\n";
        return false;
    }

    string line;
    getline(fin, line);

    while (getline(fin, line)) {
        if (line.empty()) continue;
        auto fields = splitCSV(line);
        if (fields.size() < 14) {
            cerr << "Warning: skipped line with " << fields.size() << " columns: " << line << "\n";
            continue;
        }
        Applicants.emplace_back(fields);
    }

    cout << "Loaded " << Applicants.size() << " Applicants.\n";

    sort(Applicants.begin(), Applicants.end(), [](const Applicant &a, const Applicant &b){
        return a.Rank < b.Rank;
    });
    
    return true;
}

bool ExtractingSeatMatrixCSV(){
    ifstream fin(SeatMatrixFilePath);
    if (!fin.is_open()) {
        cerr << "Error opening: " << SeatMatrixFilePath << "\n";
        return false;
    }

    string line;
    for(int i=0; i<6; i++) getline(fin, line);
    
    vector<string> fields;
    for(int i=0; i<3; i++){
        fields = splitCSV(line);
        
        for(int j=0; j<stoi(fields[1]); j++) CHSeats[i]["UR"]["OPEN"].insert("UR_OP");
        for(int j=0; j<stoi(fields[2]); j++) CHSeats[i]["UR"]["F"].insert("UR_F");
        for(int j=0; j<stoi(fields[3]); j++) CHSeats[i]["UR"]["EXG"].insert("UR_EXG");
        for(int j=0; j<stoi(fields[4]); j++) CHSeats[i]["UR"]["FF"].insert("UR_FF");
        for(int j=0; j<stoi(fields[5]); j++) CHSeats[i]["UR"]["PWD"].insert("UR_PWD");

        for(int j=0; j<stoi(fields[6]); j++) CHSeats[i]["ST"]["OPEN"].insert("ST_OP");
        for(int j=0; j<stoi(fields[7]); j++) CHSeats[i]["ST"]["F"].insert("ST_F");
        for(int j=0; j<stoi(fields[8]); j++) CHSeats[i]["ST"]["EXG"].insert("ST_EXG");
        for(int j=0; j<stoi(fields[9]); j++) CHSeats[i]["ST"]["FF"].insert("ST_FF");
        for(int j=0; j<stoi(fields[10]); j++) CHSeats[i]["ST"]["PWD"].insert("ST_PWD");
        for(int j=0; j<stoi(fields[11]); j++) CHSeats[i]["ST"]["PVTP"].insert("ST_PVTP");

        for(int j=0; j<stoi(fields[12]); j++) CHSeats[i]["SC"]["OPEN"].insert("SC_OP");
        for(int j=0; j<stoi(fields[13]); j++) CHSeats[i]["SC"]["F"].insert("SC_F");
        for(int j=0; j<stoi(fields[14]); j++) CHSeats[i]["SC"]["EXG"].insert("SC_EXG");
        for(int j=0; j<stoi(fields[15]); j++) CHSeats[i]["SC"]["FF"].insert("SC_FF");
        for(int j=0; j<stoi(fields[16]); j++) CHSeats[i]["SC"]["PWD"].insert("SC_PWD");

        for(int j=0; j<stoi(fields[17]); j++) CHSeats[i]["OBC"]["OPEN"].insert("OBC_OP");
        for(int j=0; j<stoi(fields[18]); j++) CHSeats[i]["OBC"]["F"].insert("OBC_F");
        for(int j=0; j<stoi(fields[19]); j++) CHSeats[i]["OBC"]["EXG"].insert("OBC_EXG");
        for(int j=0; j<stoi(fields[20]); j++) CHSeats[i]["OBC"]["FF"].insert("OBC_FF");
        for(int j=0; j<stoi(fields[21]); j++) CHSeats[i]["OBC"]["PWD"].insert("OBC_PWD");
        
        Capacity += stoi(fields[1]) + stoi(fields[2]) + stoi(fields[3]) + stoi(fields[4]) + stoi(fields[5]);
        Capacity += stoi(fields[6]) + stoi(fields[7]) + stoi(fields[8]) + stoi(fields[9]) + stoi(fields[10]) + stoi(fields[11]);
        Capacity += stoi(fields[12]) + stoi(fields[13]) + stoi(fields[14]) + stoi(fields[15]) + stoi(fields[16]);
        Capacity += stoi(fields[17]) + stoi(fields[18]) + stoi(fields[19]) + stoi(fields[20]) + stoi(fields[21]);
        
        getline(fin, line);
    }

    for(int i=0; i<(20-9); i++) getline(fin, line);

    for(int i=0; i<3; i++){
        fields = splitCSV(line);
        
        for(int j=0; j<stoi(fields[1]); j++) NTPCSeats[i]["UR"]["OPEN"].insert("UR_OP");
        for(int j=0; j<stoi(fields[2]); j++) NTPCSeats[i]["UR"]["PWD"].insert("UR_PWD");
        for(int j=0; j<stoi(fields[3]); j++) NTPCSeats[i]["UR"]["F"].insert("UR_F");

        for(int j=0; j<stoi(fields[5]); j++) NTPCSeats[i]["ST"]["OPEN"].insert("ST_OP");
        for(int j=0; j<stoi(fields[6]); j++) NTPCSeats[i]["ST"]["PWD"].insert("ST_PWD");
        for(int j=0; j<stoi(fields[7]); j++) NTPCSeats[i]["ST"]["F"].insert("ST_F");

        for(int j=0; j<stoi(fields[9]); j++) NTPCSeats[i]["SC"]["OPEN"].insert("SC_OP");
        for(int j=0; j<stoi(fields[10]); j++) NTPCSeats[i]["SC"]["PWD"].insert("SC_PWD");
        for(int j=0; j<stoi(fields[11]); j++) NTPCSeats[i]["SC"]["F"].insert("SC_F");

        for(int j=0; j<stoi(fields[13]); j++) NTPCSeats[i]["OBC"]["OPEN"].insert("OBC_OP");
        for(int j=0; j<stoi(fields[14]); j++) NTPCSeats[i]["OBC"]["PWD"].insert("OBC_PWD");
        for(int j=0; j<stoi(fields[15]); j++) NTPCSeats[i]["OBC"]["F"].insert("OBC_F");

        for(int j=0; j<stoi(fields[17]); j++) NTPCSeats[i]["EWS"]["OPEN"].insert("EWS_OP");
        for(int j=0; j<stoi(fields[18]); j++) NTPCSeats[i]["EWS"]["PWD"].insert("EWS_PWD");
        for(int j=0; j<stoi(fields[19]); j++) NTPCSeats[i]["EWS"]["F"].insert("EWS_F");
        
        Capacity += stoi(fields[1]) + stoi(fields[2]) + stoi(fields[3]);
        Capacity += stoi(fields[5]) + stoi(fields[6]) + stoi(fields[7]);
        Capacity += stoi(fields[9]) + stoi(fields[10]) + stoi(fields[11]);
        Capacity += stoi(fields[13]) + stoi(fields[14]) + stoi(fields[15]);
        Capacity += stoi(fields[17]) + stoi(fields[18]) + stoi(fields[19]);
        
        getline(fin, line);
    }
    
    cout << "Seat Matrix Extraction Complete.\n";
    return true;
}

bool AllotChoice(Applicant &Applicant, int Branch, string &Category, string &Quota){
    if(Branch < 0 || Branch > 2) return false;

    multiset<string> *ptrSet = nullptr;
    string prefix;
    
    if(Applicant.AppliedQuota == 1) prefix = "CG_";
    else if(Applicant.AppliedQuota == 2) prefix = "NTPC_";
    else prefix = "KM";

    if(Iteration == 1){
        if(Applicant.AppliedQuota == 1) ptrSet = &CHSeats[Branch][Category][Quota];
        else if(Applicant.AppliedQuota == 2) ptrSet = &NTPCSeats[Branch][Category][Quota];
        else if(Applicant.AppliedQuota == 3) {
            if(KashmiriSeats[IntToBranch[Branch]] > 0) {
                KashmiriSeats[IntToBranch[Branch]]--;
                
                if(Applicant.AllottedBranch != -1 && Applicant.AllottedBranch != Branch){
                    KashmiriSeats[IntToBranch[Applicant.AllottedBranch]]++;
                } else SeatsFilled++;
                
                Applicant.AllottedBranch = Branch;
                Applicant.AllottedSeat = prefix;
                Applicant.AllottedCategory = Category;
                Applicant.AllottedQuota = Quota;
                return true;
            }
            return false;
        }
    } else if(Applicant.AppliedQuota == 3) {
        return false;
    } else if(Iteration == 2){
        if(Applicant.AppliedQuota == 1) ptrSet = &CHSeats[Branch][Category][Quota];
        else if(Applicant.Quota == "OPEN") ptrSet = &NTPCSeats[Branch][Category][Quota];
    } else if(Iteration == 3 && Category == "ST"){
        ptrSet = (Applicant.AppliedQuota == 1) ? &CHSeats[Branch]["ST"][Quota] : &NTPCSeats[Branch]["ST"][Quota];
    } else if(Iteration == 4 && Category == "SC"){
        ptrSet = (Applicant.AppliedQuota == 1) ? &CHSeats[Branch]["SC"][Quota] : &NTPCSeats[Branch]["SC"][Quota];
    } else if(Iteration == 5 && Category == "ST"){
        ptrSet = (Applicant.AppliedQuota == 1) ? &CHSeats[Branch]["ST"][Quota] : &NTPCSeats[Branch]["ST"][Quota];
    } else if(Iteration == 6 && Category == "OBC"){
        ptrSet = (Applicant.AppliedQuota == 1) ? &CHSeats[Branch]["OBC"][Quota] : &NTPCSeats[Branch]["OBC"][Quota];
    } else if(Iteration == 7 && Category == "UR"){
        ptrSet = (Applicant.AppliedQuota == 1) ? &CHSeats[Branch]["UR"][Quota] : &NTPCSeats[Branch]["UR"][Quota];
    } else if(Iteration == 8 && Applicant.AppliedQuota == 2){
        ptrSet = &NTPCSeats[Branch]["UR"][Quota];
    }

    if(ptrSet && !ptrSet->empty()){
        string seatLabel = *ptrSet->begin();
        ptrSet->erase(ptrSet->begin());

        // if applicant already had a seat
        if(Applicant.AllottedBranch != -1 && Applicant.AllottedBranch != Branch){
            if(Applicant.AppliedQuota == 1) {
                CHSeats[Applicant.AllottedBranch][Applicant.AllottedCategory][Applicant.AllottedQuota].insert(Applicant.AllottedSeatLabel);
            } else if(Applicant.AppliedQuota == 2) {
                NTPCSeats[Applicant.AllottedBranch][Applicant.AllottedCategory][Applicant.AllottedQuota].insert(Applicant.AllottedSeatLabel);
            }
        } else {
            SeatsFilled++;
        }

        size_t underscorePos = seatLabel.find('_');
        string originalCategory = seatLabel.substr(0, underscorePos);
        string originalQuota = seatLabel.substr(underscorePos + 1);
        string requestedCategory = Category;
        string requestedQuotaStr = Quota;
        string temp;

        originalQuota = (originalQuota == "OPEN") ? "OP" : originalQuota;
        requestedQuotaStr = (requestedQuotaStr == "OPEN") ? "OP" : requestedQuotaStr;
        originalQuota = (originalQuota == "EXG") ? "ExS" : originalQuota;
        requestedQuotaStr = (requestedQuotaStr == "EXG") ? "ExS" : requestedQuotaStr;
        originalCategory = (originalCategory == "OBC") ? "OBCNCL" : originalCategory;
        requestedCategory = (requestedCategory == "OBC") ? "OBCNCL" : requestedCategory;

        // Case 1: Category is different (allotted vs requested)
        if(originalCategory != requestedCategory && originalQuota != requestedQuotaStr){
            temp = prefix + originalCategory + "_" + originalQuota + "-->" + requestedCategory + "_" + requestedQuotaStr;
        }else if(originalCategory != requestedCategory) {
            temp = prefix + originalCategory+ "-->" + requestedCategory + "_" + requestedQuotaStr;
        }
        // Case 2: Category is same, quota is different
        else if(originalQuota != requestedQuotaStr) {
            temp = prefix + requestedCategory + "_" + originalQuota + "-->" + requestedQuotaStr;
        }
        // Case 3: Both category and quota are same
        else {
            temp = prefix + requestedCategory + "_" + requestedQuotaStr;
        }

        Applicant.AllottedBranch = Branch;
        Applicant.AllottedSeat = temp;
        Applicant.AllottedCategory = Category;
        Applicant.AllottedQuota = Quota;
        Applicant.AllottedSeatLabel = seatLabel; // Store for deallocation

        return true;
    }
    return false;
}

void MergeAllotments(){
    int cnt = 1;
    for(Applicant &x: Applicants){
        if(x.AllottedBranch != -1){
            vector<string> temp;
            temp.push_back(to_string(cnt++));
            temp.push_back(to_string(x.ApplicationNumber));
            temp.push_back(x.Name);
            temp.push_back(x.EmailId);
            temp.push_back(to_string(x.Rank));
            temp.push_back(IntToBranch[x.AllottedBranch]);
            temp.push_back(x.AllottedSeat);
            
            MergedAllotments.push_back(temp);
        }
    }
}

void Iterate(){
    cout << "Iteration Number: " << Iteration << "\n";

    for (ll i = 0; i < Applicants.size() && SeatsFilled < Capacity; i++){
        int op = 0;
        vector<pair<string,string>> *LogicLine;
        
        if(Applicants[i].AppliedQuota == 1) {
            LogicLine = &LogicLines[Applicants[i].Category][Applicants[i].Gender][Applicants[i].Quota];
        } else if(Applicants[i].AppliedQuota == 2) {
            LogicLine = &NTPCLogicLines[Applicants[i].Category][Applicants[i].Gender][Applicants[i].Quota == "PWD" ? "PWD" : "OPEN"];
        } else {
            if(AllotChoice(Applicants[i], Applicants[i].Choice1, Applicants[i].Category, Applicants[i].Quota)){}
            else if(AllotChoice(Applicants[i], Applicants[i].Choice2, Applicants[i].Category, Applicants[i].Quota)){}
            else if(AllotChoice(Applicants[i], Applicants[i].Choice3, Applicants[i].Category, Applicants[i].Quota)){}
            continue;
        }

        op = Applicants[i].AllottedBranch == Applicants[i].Choice1;
        if(op) continue;
        for(auto &x : *LogicLine){
            if(AllotChoice(Applicants[i], Applicants[i].Choice1, x.first, x.second)){
                op = 1;
                break;
            }
        }
        
        op |= Applicants[i].AllottedBranch == Applicants[i].Choice2;
        if(op) continue;
        for(auto &x : *LogicLine){
            if(AllotChoice(Applicants[i], Applicants[i].Choice2, x.first, x.second)){
                op = 1;
                break;
            }
        }
        
        op |= Applicants[i].AllottedBranch == Applicants[i].Choice3;
        if(op) continue;
        for(auto &x : *LogicLine){
            if(AllotChoice(Applicants[i], Applicants[i].Choice3, x.first, x.second)){
                op = 1;
                break;
            }
        }
    }
    
    cout << "Allocation Complete. Seats Filled: " << SeatsFilled << " Out of " << Capacity << "\n\n";
    Iteration++;
}

void MainLogic(){
    Iterate();
    
    // Transfer seats from special quotas to OPEN for CG
    for(int i=0; i<3; i++){
        for(auto &x: CHSeats[i]){
            for(auto &y: x.second){
                if(y.first == "OPEN") continue;
                // Move all seats from special quota to OPEN
                CHSeats[i][x.first]["OPEN"].insert(y.second.begin(), y.second.end());
                y.second.clear();
            }
        }
    }
    
    // PWD to OPEN/F for NTPC
    for(int i=0; i<3; i++){
        for(auto &x: NTPCSeats[i]){
            for(auto &y: x.second){
                if(y.first == "OPEN" || y.first == "F") continue;
                // PWD to OPEN
                NTPCSeats[i][x.first]["OPEN"].insert(y.second.begin(), y.second.end());
                y.second.clear();
            }
        }
    }
    
    Iterate();
    
    // SC to ST
    for(int i=0; i<3; i++){
        CHSeats[i]["ST"]["OPEN"].insert(CHSeats[i]["SC"]["OPEN"].begin(), CHSeats[i]["SC"]["OPEN"].end());
        CHSeats[i]["SC"]["OPEN"].clear();
        
        NTPCSeats[i]["ST"]["OPEN"].insert(NTPCSeats[i]["SC"]["OPEN"].begin(), NTPCSeats[i]["SC"]["OPEN"].end());
        NTPCSeats[i]["SC"]["OPEN"].clear();
        
        NTPCSeats[i]["ST"]["F"].insert(NTPCSeats[i]["SC"]["F"].begin(), NTPCSeats[i]["SC"]["F"].end());
        NTPCSeats[i]["SC"]["F"].clear();
    }
    
    Iterate();
    
    // ST to SC
    for(int i=0; i<3; i++){
        CHSeats[i]["SC"]["OPEN"].insert(CHSeats[i]["ST"]["OPEN"].begin(), CHSeats[i]["ST"]["OPEN"].end());
        CHSeats[i]["ST"]["OPEN"].clear();
        
        NTPCSeats[i]["SC"]["OPEN"].insert(NTPCSeats[i]["ST"]["OPEN"].begin(), NTPCSeats[i]["ST"]["OPEN"].end());
        NTPCSeats[i]["ST"]["OPEN"].clear();
        
        NTPCSeats[i]["SC"]["F"].insert(NTPCSeats[i]["ST"]["F"].begin(), NTPCSeats[i]["ST"]["F"].end());
        NTPCSeats[i]["ST"]["F"].clear();
    }
    
    Iterate();
    
    // SC to ST again
    for(int i=0; i<3; i++){
        CHSeats[i]["ST"]["OPEN"].insert(CHSeats[i]["SC"]["OPEN"].begin(), CHSeats[i]["SC"]["OPEN"].end());
        CHSeats[i]["SC"]["OPEN"].clear();
        
        NTPCSeats[i]["ST"]["OPEN"].insert(NTPCSeats[i]["SC"]["OPEN"].begin(), NTPCSeats[i]["SC"]["OPEN"].end());
        NTPCSeats[i]["SC"]["OPEN"].clear();
        
        NTPCSeats[i]["ST"]["F"].insert(NTPCSeats[i]["SC"]["F"].begin(), NTPCSeats[i]["SC"]["F"].end());
        NTPCSeats[i]["SC"]["F"].clear();
    }
    
    Iterate();
    
    // ST to OBC
    for(int i=0; i<3; i++){
        CHSeats[i]["OBC"]["OPEN"].insert(CHSeats[i]["ST"]["OPEN"].begin(), CHSeats[i]["ST"]["OPEN"].end());
        CHSeats[i]["ST"]["OPEN"].clear();
        
        NTPCSeats[i]["OBC"]["OPEN"].insert(NTPCSeats[i]["ST"]["OPEN"].begin(), NTPCSeats[i]["ST"]["OPEN"].end());
        NTPCSeats[i]["ST"]["OPEN"].clear();
        
        NTPCSeats[i]["OBC"]["F"].insert(NTPCSeats[i]["ST"]["F"].begin(), NTPCSeats[i]["ST"]["F"].end());
        NTPCSeats[i]["ST"]["F"].clear();
    }
    
    Iterate();
    
    // OBC to UR
    for(int i=0; i<3; i++){
        CHSeats[i]["UR"]["OPEN"].insert(CHSeats[i]["OBC"]["OPEN"].begin(), CHSeats[i]["OBC"]["OPEN"].end());
        CHSeats[i]["OBC"]["OPEN"].clear();
        
        NTPCSeats[i]["UR"]["OPEN"].insert(NTPCSeats[i]["OBC"]["OPEN"].begin(), NTPCSeats[i]["OBC"]["OPEN"].end());
        NTPCSeats[i]["OBC"]["OPEN"].clear();
        
        NTPCSeats[i]["UR"]["F"].insert(NTPCSeats[i]["OBC"]["F"].begin(), NTPCSeats[i]["OBC"]["F"].end());
        NTPCSeats[i]["OBC"]["F"].clear();
    }
    
    Iterate();
    
    //  EWS to UR for NTPC
    for(int i=0; i<3; i++){
        NTPCSeats[i]["UR"]["OPEN"].insert(NTPCSeats[i]["EWS"]["OPEN"].begin(), NTPCSeats[i]["EWS"]["OPEN"].end());
        NTPCSeats[i]["EWS"]["OPEN"].clear();
        
        NTPCSeats[i]["UR"]["F"].insert(NTPCSeats[i]["EWS"]["F"].begin(), NTPCSeats[i]["EWS"]["F"].end());
        NTPCSeats[i]["EWS"]["F"].clear();
    }
    
    Iterate();
}

void WriteAllotmentsToCSV(){
    ofstream fout(AllotmentsFilePath);

    if (!fout.is_open()) {
        cout << "Error opening file: " << AllotmentsFilePath << endl;
        return;
    }

    fout << "S No.,Application Number,Name,EmailId,Rank,Branch,Allotted Category\n";

    for (const auto& row : MergedAllotments) {
        for (size_t i = 0; i < row.size(); ++i) {
            fout << row[i];
            if (i != row.size() - 1) fout << ",";
        }
        fout << "\n";
    }

    fout.close();
    cout << "Conversion to CSV completed.\n";
}

int main(){
    if(!ExtractingDataFromCSV()) return 0;
    if(!ExtractingSeatMatrixCSV()) return 0;
    
    MainLogic();
    
    MergeAllotments();
    WriteAllotmentsToCSV();
    
    return 0;
}