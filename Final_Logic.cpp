#include <bits/stdc++.h>
using namespace std;

string ApplicantsFilePath = "./Cleaned Applicants.csv";
string SeatMatrixFilePath = "./new_seat_matrix.csv";
string AllotmentsFilePath = "./allotted.csv";
typedef long long   ll;

int mapChoice(const string &s) {
    if (s == "CSE")  return 0;
    else if (s == "DSAI") return 1;
    else if (s == "ECE")  return 2;
    else return 0;  // N/A or unknown
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
    // Members = 20
    ll SerialNumber;
    ll ApplicationNumber;
    string Name;
    ll Rank;
    string Category; // UR, SC, ST, OBC
    string Quota; // EXG, FF, PWD
    string Gender;
    string EmailId;
    int Choice1; // CSE: 0, DSAI: 1, ECE: 2
    int Choice2;
    int Choice3;

    bool IsOpen;      // Male
    bool IsFemale;    // Female
    bool IsEXG;       // ex‐serviceman
    bool IsPWD;       // Physically disabled
    bool IsFF;        // Freedom fighter
    bool IsPVTP;      // always false for now

    bool IsUR;
    bool IsSC;
    bool IsST;
    bool IsOBC;
    bool IsEWS;

    int AppliedQuota; // Chattisgarh: 1, NTPC: 2, Kashmiri: 3
    int AllottedBranch = -1;
    string AllottedSeat;
    string AllottedCategory;
    string AllottedQuota;

    Applicant(const vector<string> &f) {
        SerialNumber      = stoll(f[0]);
        ApplicationNumber = stoll(f[1]);
        Name              = f[2];
        EmailId           = f[13];
        string gender     = f[3];
        string category   = f[4];
        if(category == "OBC NCL" || category == "OBC-NCL") category = "OBC";

        Choice1           = mapChoice(f[5]);
        Choice2           = mapChoice(f[6]);
        Choice3           = mapChoice(f[7]);
        Rank              = stoll(f[8]);
        AppliedQuota      = mapQuota(f[9]);

        IsOpen    = (gender == "Male");
        IsFemale  = (gender == "Female");
        if(IsOpen) Gender = "M";
        else Gender = "F";

        IsPWD = yesNoToBool(f[10]);
        IsEXG = yesNoToBool(f[11]);
        IsFF  = yesNoToBool(f[12]);

        IsPVTP = false;

        IsUR  = (category == "UR");
        IsSC  = (category == "SC");
        IsST  = (category == "ST");
        IsEWS = (category == "EWS");
        IsOBC = (category == "OBC");
        Category = category;
        if(IsPWD) Quota = "PWD";
        else if(IsEXG) Quota = "EXG";
        else if(IsFF) Quota = "FF";
        else Quota = "OPEN";

        AllottedCategory = "";
    }

    void DisplayApplicant() {
        cout << "S No.: " << this->SerialNumber << ", ";
        cout << "Appn No.: " << this->ApplicationNumber << ", ";
        cout << "Name: " << this->Name << ", ";
        cout << "EMail-ID: " << this->EmailId << ", ";
        cout << "Rank: " << this->Rank << ", ";
        cout << "Preferences: " << this->Choice1 << " " << this->Choice2 << " " << this->Choice3 << ", ";
        cout << "Applied Quota: " << this->AppliedQuota << ", ";
        cout << "Category: " << this->Category << ", ";
        cout << "Gender: " << this->Gender << ", ";
        cout << "Quota: " << this->Quota << ", ";
        cout << "Allotted Branch: " << this->AllottedBranch << ", ";
        cout << "Allotted Seat: " << this->AllottedSeat << ", ";
        // cout << "IsOpen: " << this->IsOpen << "\t";
        // cout << "IsFemale: " << this->IsFemale << "\t";
        // cout << "IsEXG: " << this->IsEXG << "\t";
        // cout << "IsPWD: " << this->IsPWD << "\t";
        // cout << "IsFF: " << this->IsFF << "\t";
        // cout << "IsPVTP: " << this->IsPVTP << "\t";
        // cout << "IsUR: " << this->IsUR << "\t";
        // cout << "IsSC: " << this->IsSC << "\t";
        // cout << "IsST: " << this->IsST << "\t";
        // cout << "IsOBC: " << this->IsOBC << "\t";
        // cout << "IsEWS: " << this->IsEWS << "\t";
        // cout << "AppliedQuota: " << this->AppliedQuota << "\t";
        // cout << "AllottedCategory: " << this->AllottedCategory << "\t";
        cout << "\n";
    }  
};

int SeatsFilled = 0, Capacity = 3, Iteration = 1;
vector<Applicant> Applicants;
vector< unordered_map<string, unordered_map<string, int>> > CHSeats(3), NTPCSeats(3); // CHSeats[i][Category][Quota] = Vacant seats of Branch (Seats), taken from seat_matrix.csv
vector< unordered_map<string, unordered_map<string, vector<Applicant>>> > Allotments(3); //Stores Final Allotments according to Category and Quota
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
            { "EXG" , { {"UR", "F"}, {"UR", "EXG"}, {"UR", "OPEN"} } }, // Swapped (now it is as proper Logic Line)
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
vector< vector<string> > MergedAllotments;
map<int, string> IntToBranch = { {0, "CSE"}, {1, "DSAI"}, {2, "ECE"} };

vector<string> splitCSV(const string &line) {
    vector<string> out;
    string cur;
    // simple split on commas (no quoted commas in your data)
    stringstream ss(line);
    while (getline(ss, cur, ',')) {
        out.push_back(cur);
    }
    return out;
}

bool ExtractingDataFromCSV()
{
    ifstream fin(ApplicantsFilePath);
    if (!fin.is_open())
    {
        cerr << "Error opening: " <<ApplicantsFilePath <<"\n";
        return false;
    }

    string line;
    // skip header
    getline(fin, line);

    while (getline(fin, line))
    {
        if (line.empty()) continue;
        auto fields = splitCSV(line);
        if (fields.size() < 14)
        {
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
        cerr << "Error opening: " <<SeatMatrixFilePath <<"\n";
        return false;
    }

    string line;
    for(int i=0; i<6; i++) getline(fin, line);
    // cout <<line <<"\n";
    vector<string> fields;
    for(int i=0; i<3; i++){
        fields = splitCSV(line);
        CHSeats[i]["UR"]["OPEN"] = stoi(fields[1]), Capacity += stoi(fields[1]);
        CHSeats[i]["UR"]["F"] = stoi(fields[2]), Capacity += stoi(fields[2]);
        CHSeats[i]["UR"]["EXG"] = stoi(fields[3]), Capacity += stoi(fields[3]);
        CHSeats[i]["UR"]["FF"] = stoi(fields[4]), Capacity += stoi(fields[4]);
        CHSeats[i]["UR"]["PWD"] = stoi(fields[5]), Capacity += stoi(fields[5]);

        CHSeats[i]["ST"]["OPEN"] = stoi(fields[6]), Capacity += stoi(fields[6]);
        CHSeats[i]["ST"]["F"] = stoi(fields[7]), Capacity += stoi(fields[7]);
        CHSeats[i]["ST"]["EXG"] = stoi(fields[8]), Capacity += stoi(fields[8]);
        CHSeats[i]["ST"]["FF"] = stoi(fields[9]), Capacity += stoi(fields[9]);
        CHSeats[i]["ST"]["PWD"] = stoi(fields[10]), Capacity += stoi(fields[10]);
        CHSeats[i]["ST"]["PVTP"] = stoi(fields[11]), Capacity += stoi(fields[11]);

        CHSeats[i]["SC"]["OPEN"] = stoi(fields[12]), Capacity += stoi(fields[12]);
        CHSeats[i]["SC"]["F"] = stoi(fields[13]), Capacity += stoi(fields[13]);
        CHSeats[i]["SC"]["EXG"] = stoi(fields[14]), Capacity += stoi(fields[14]);
        CHSeats[i]["SC"]["FF"] = stoi(fields[15]), Capacity += stoi(fields[15]);
        CHSeats[i]["SC"]["PWD"] = stoi(fields[16]), Capacity += stoi(fields[16]);

        CHSeats[i]["OBC"]["OPEN"] = stoi(fields[17]), Capacity += stoi(fields[17]);
        CHSeats[i]["OBC"]["F"] = stoi(fields[18]), Capacity += stoi(fields[18]);
        CHSeats[i]["OBC"]["EXG"] = stoi(fields[19]), Capacity += stoi(fields[19]);
        CHSeats[i]["OBC"]["FF"] = stoi(fields[20]), Capacity += stoi(fields[20]);
        CHSeats[i]["OBC"]["PWD"] = stoi(fields[21]), Capacity += stoi(fields[21]);
        getline(fin, line);
    }

    //Now on 9th line
    for(int i=0; i<(20-9); i++) getline(fin, line);

    for(int i=0; i<3; i++){
        fields = splitCSV(line);
        NTPCSeats[i]["UR"]["OPEN"] = stoi(fields[1]), Capacity+= stoi(fields[1]);
        NTPCSeats[i]["UR"]["PWD"] = stoi(fields[2]), Capacity+= stoi(fields[2]);
        NTPCSeats[i]["UR"]["F"] = stoi(fields[3]), Capacity+= stoi(fields[3]);

        NTPCSeats[i]["ST"]["OPEN"] = stoi(fields[5]), Capacity+= stoi(fields[5]);
        NTPCSeats[i]["ST"]["PWD"] = stoi(fields[6]), Capacity+= stoi(fields[6]);
        NTPCSeats[i]["ST"]["F"] = stoi(fields[7]), Capacity+= stoi(fields[7]);

        NTPCSeats[i]["SC"]["OPEN"] = stoi(fields[9]), Capacity+= stoi(fields[9]);
        NTPCSeats[i]["SC"]["PWD"] = stoi(fields[10]), Capacity+= stoi(fields[10]);
        NTPCSeats[i]["SC"]["F"] = stoi(fields[11]), Capacity+= stoi(fields[11]);

        NTPCSeats[i]["OBC"]["OPEN"] = stoi(fields[13]), Capacity+= stoi(fields[13]);
        NTPCSeats[i]["OBC"]["PWD"] = stoi(fields[14]), Capacity+= stoi(fields[14]);
        NTPCSeats[i]["OBC"]["F"] = stoi(fields[15]), Capacity+= stoi(fields[15]);

        NTPCSeats[i]["EWS"]["OPEN"] = stoi(fields[17]), Capacity+= stoi(fields[17]);
        NTPCSeats[i]["EWS"]["PWD"] = stoi(fields[18]), Capacity+= stoi(fields[18]);
        NTPCSeats[i]["EWS"]["F"] = stoi(fields[19]), Capacity+= stoi(fields[19]);
        getline(fin, line);
    }
    cout <<"Seat Matrix Extraction Complete.\n";
    return true;
}

void displaySeatMatrix(){
    cout << "CG Seat Matrix:\n";
    cout << "Branch\tCategory\tQuota\tSeats\n";
    for(int i=0; i<3; i++){
        for(auto &cat : CHSeats[i]){
            for(auto &q : cat.second){
                cout << i+1 << "\t" << cat.first << "\t" << q.first << "\t" << q.second << "\n";
            }
        }
    }
    cout <<"\n\n";

    cout << "NTPC Seat Matrix:\n";
    cout << "Branch\tCategory\tQuota\tSeats\n";
    for(int i=0; i<3; i++){
        for(auto &cat : NTPCSeats[i]){
            for(auto &q : cat.second){
                cout << i+1 << "\t" << cat.first << "\t" << q.first << "\t" << q.second << "\n";
            }
        }
    }
    cout <<"\n\n";
    cout << "Kashmiri Migrant Seat Matrix:\n";
    cout << "Branch\tSeats\n";
    for(auto &x: KashmiriSeats){
        cout <<x.first <<"\t" <<x.second <<"\n";
    }
    cout <<"\n\n";
}

void displayApplicants() {
    for(int i = 0; i<20; i++){
        Applicants[i].DisplayApplicant();
    }
}

void DisplayAllotments(){
    vector<string> branches = {"CSE", "DSAI", "ECE"};

    for (int i = 0; i < 3; ++i) {
        cout << "Branch: " << branches[i] << "\n";

        for (const auto& catPair : Allotments[i]) {
            const string& category = catPair.first;
            cout << "  Category: " << category << "\n";

            for (const auto& quotaPair : catPair.second) {
                const string& quota = quotaPair.first;
                cout << "    Quota: " << quota << "\n";

                for (const Applicant& app : quotaPair.second) {
                    cout << "      - " << app.Name <<", " <<app.Rank << "\n";
                }
            }
        }
        cout << "----------------------------------\n";
    }
}

void DisplayAllotmentList(){
    cout <<"S No.\tApplication No.\tName\tEmailId\tJEE Mains CRL\tBranch Allotted\tAllotted Caregory\t\n";
    for(auto &app: MergedAllotments){
        for(auto &field: app){
            cout <<field <<"\t";
        }
        cout <<"\n";
    }
}

bool AllotChoice(Applicant &Applicant, int Branch, string &Category, string &Quota){
    if(Branch<0 || Branch>2) return false;

    int *ptr = NULL;
    string temp;
    if(Applicant.AppliedQuota == 1) temp = "CG_";
    else if(Applicant.AppliedQuota == 2) temp = "NTPC_";
    else temp = "KM";

    if(Iteration==1){
        if(Applicant.AppliedQuota == 1) ptr = &CHSeats[Branch][Category][Quota], temp+=  Category + "_" + (Quota=="OPEN"?"OP":Quota) ;
        else if(Applicant.AppliedQuota == 2) ptr = &NTPCSeats[Branch][Category][Quota], temp+=  Category + "_" + (Quota=="OPEN"?"OP":Quota) ;
        else if(Applicant.AppliedQuota == 3) ptr = &KashmiriSeats[ IntToBranch[Branch] ];
    }else if(Applicant.AppliedQuota == 3) return false;
    else if(Iteration==2){
        if(Applicant.AppliedQuota == 1){
            ptr = &CHSeats[Branch][Category][Quota];
            temp+=  Category + "_XX-->" + (Quota=="OPEN"?"OP":Quota);
        }else if(Applicant.Quota=="OPEN"){
            ptr = &NTPCSeats[Branch][Category][Quota];
            temp+=  Category + "_PwD-->OP";
        }
    }else if(Iteration==3 && Category=="ST"){
        ptr = &CHSeats[Branch]["ST"][Quota];
        if(Applicant.AppliedQuota == 2) ptr = &NTPCSeats[Branch]["ST"][Quota];
        
        temp+=  ("SC-->ST_") + (Quota=="OPEN"?"OP":Quota);
    }else if(Iteration==4 && Category=="SC"){
        ptr = &CHSeats[Branch]["SC"][Quota];
        if(Applicant.AppliedQuota == 2) ptr = &NTPCSeats[Branch]["SC"][Quota];
        
        temp+=  ("ST-->SC_") + (Quota=="OPEN"?"OP":Quota);
    }else if(Iteration==5 && Category=="ST"){
        ptr = &CHSeats[Branch]["ST"][Quota];
        if(Applicant.AppliedQuota == 2) ptr = &NTPCSeats[Branch]["ST"][Quota];
        
        temp+=  ("SC-->ST_") + (Quota=="OPEN"?"OP":Quota);
    }else if(Iteration==6 && (Category=="OBC") ){
        ptr = &CHSeats[Branch]["OBC"][Quota];
        if(Applicant.AppliedQuota == 2) ptr = &NTPCSeats[Branch]["OBC"][Quota];
        
        temp+=  ("ST-->OBC_") + (Quota=="OPEN"?"OP":Quota);
    }else if(Iteration==7 && Category=="UR"){
        ptr = &CHSeats[Branch]["UR"][Quota];
        if(Applicant.AppliedQuota == 2) ptr = &NTPCSeats[Branch]["UR"][Quota];
        
        temp+=  ("OBC-->UR_") + (Quota=="OPEN"?"OP":Quota);
    }else if(Iteration==8 && Applicant.AppliedQuota == 2){
        ptr = &NTPCSeats[Branch]["UR"][Quota];
        
        temp+=  ("EWS-->UR_") + (Quota=="OPEN"?"OP":Quota);
    }

    if( ptr && *ptr ){
        (*ptr)--;

        if(Applicant.AllottedBranch!=-1 && Applicant.AllottedBranch!=Branch){
            if(Applicant.AppliedQuota == 1) CHSeats[Applicant.AllottedBranch][Applicant.AllottedCategory][Applicant.AllottedQuota]++;
            else if(Applicant.AppliedQuota == 2) NTPCSeats[Applicant.AllottedBranch][Applicant.AllottedCategory][Applicant.AllottedQuota]++;
        }else SeatsFilled++;
        
        Applicant.AllottedBranch = Branch;
        Applicant.AllottedSeat = temp;
        Applicant.AllottedCategory = Category;
        Applicant.AllottedQuota = Quota;

        // if(Applicant.AppliedQuota == 2) Applicant.DisplayApplicant();
        return true;
    }
    return false;
}

void MergeAllotments(){
    int cnt = 1;
    for(Applicant &x: Applicants){
        if(x.AllottedBranch!=-1){
            vector<string> temp;

            temp.push_back( to_string(cnt++) );
            temp.push_back( to_string(x.ApplicationNumber) );
            temp.push_back( x.Name );
            temp.push_back( x.EmailId );
            temp.push_back( to_string(x.Rank) );
            temp.push_back( IntToBranch[x.AllottedBranch] );
            temp.push_back( x.AllottedSeat );
            
            MergedAllotments.push_back( temp );
        }
    }
}

void Iterate(){
    cout <<"Iteration Number: " <<Iteration <<"\n";

    // cerr <<SeatsFilled <<" " <<Capacity <<"\n";
    for (ll i = 0; i < Applicants.size() && SeatsFilled < Capacity; i++){
        int op = 0;
        vector< pair<string,string> > *LogicLine;
        if(Applicants[i].AppliedQuota == 1) LogicLine = &LogicLines[Applicants[i].Category][Applicants[i].Gender][Applicants[i].Quota];
        else if(Applicants[i].AppliedQuota == 2) LogicLine = &NTPCLogicLines[Applicants[i].Category][Applicants[i].Gender][ Applicants[i].Quota == "PWD"? "PWD": "OPEN" ];
        else{
            if( AllotChoice(Applicants[i], Applicants[i].Choice1, Applicants[i].Category, Applicants[i].Quota) ){}            
            else if( AllotChoice(Applicants[i], Applicants[i].Choice2, Applicants[i].Category, Applicants[i].Quota) ){}            
            else if( AllotChoice(Applicants[i], Applicants[i].Choice3, Applicants[i].Category, Applicants[i].Quota) ){}
            continue;
        }
        
        // for(auto &x:*LogicLine) cout <<x.first <<" " <<x.second <<", ";
        // cout <<"\n";
        op = Applicants[i].AllottedBranch == Applicants[i].Choice1;
        if(op) continue;
        for(auto &x:*LogicLine){
            if(AllotChoice(Applicants[i], Applicants[i].Choice1, x.first, x.second) ){
                op=1;
                break;
            }
        }
        op |= Applicants[i].AllottedBranch == Applicants[i].Choice2;
        if(op) continue;
        for(auto &x:*LogicLine){
            if( AllotChoice(Applicants[i], Applicants[i].Choice2, x.first, x.second) ){
                op=1;
                break;
            }
        }
        op |= Applicants[i].AllottedBranch == Applicants[i].Choice3;
        if(op) continue;
        for(auto &x:*LogicLine){
            if( AllotChoice(Applicants[i], Applicants[i].Choice3, x.first, x.second) ){
                op=1;
                break;
            }
        }
        // cerr <<i <<", ";
        // if(op) cerr <<"Seat Allotted for: " <<Applicants[i].Name <<"\n\n";
        // else cerr <<"Seat Not Allotted for: " <<Applicants[i].Name <<"\n\n";
    }
    cout <<"Allocation Complete. " <<"Seats Filled: " <<SeatsFilled <<" Out of " <<Capacity <<"\n\n";
    Iteration++;
}

void MainLogic(){
    Iterate();
    
    for(int i=0; i<3; i++){
        for(auto &x: CHSeats[i]){
            for(auto &y: x.second){
                if(y.first=="OPEN") continue;
                CHSeats[i][x.first]["OPEN"]+= exchange(y.second, 0);
            }
        }
    }
    for(int i=0; i<3; i++){
        for(auto &x: NTPCSeats[i]){
            for(auto &y: x.second){
                if(y.first=="OPEN" || y.first=="F") continue;
                NTPCSeats[i][x.first]["OPEN"]+= exchange(y.second, 0);
            }
        }
    }
    Iterate();
    for(int i=0; i<3; i++){
        CHSeats[i]["ST"]["OPEN"]+= exchange(CHSeats[i]["SC"]["OPEN"], 0);
        NTPCSeats[i]["ST"]["OPEN"]+= exchange(NTPCSeats[i]["SC"]["OPEN"], 0);
        NTPCSeats[i]["ST"]["F"]+= exchange(NTPCSeats[i]["SC"]["F"], 0);
    }
    Iterate();
    for(int i=0; i<3; i++){
        CHSeats[i]["SC"]["OPEN"]+= exchange(CHSeats[i]["ST"]["OPEN"], 0);
        NTPCSeats[i]["SC"]["OPEN"]+= exchange(NTPCSeats[i]["ST"]["OPEN"], 0);
        NTPCSeats[i]["SC"]["F"]+= exchange(NTPCSeats[i]["ST"]["F"], 0);
    }
    Iterate(); 
    for(int i=0; i<3; i++){
        CHSeats[i]["ST"]["OPEN"]+= exchange(CHSeats[i]["SC"]["OPEN"], 0);
        NTPCSeats[i]["ST"]["OPEN"]+= exchange(NTPCSeats[i]["SC"]["OPEN"], 0);
        NTPCSeats[i]["ST"]["F"]+= exchange(NTPCSeats[i]["SC"]["F"], 0);
    }
    Iterate();
    for(int i=0; i<3; i++){
        CHSeats[i]["OBC"]["OPEN"]+= exchange(CHSeats[i]["ST"]["OPEN"], 0);
        NTPCSeats[i]["OBC"]["OPEN"]+= exchange(NTPCSeats[i]["ST"]["OPEN"], 0);
        NTPCSeats[i]["OBC"]["F"]+= exchange(NTPCSeats[i]["ST"]["F"], 0);
    }
    Iterate();    
    for(int i=0; i<3; i++){
        CHSeats[i]["UR"]["OPEN"]+= exchange(CHSeats[i]["OBC"]["OPEN"], 0);
        NTPCSeats[i]["UR"]["OPEN"]+= exchange(NTPCSeats[i]["OBC"]["OPEN"], 0);
        NTPCSeats[i]["UR"]["F"]+= exchange(NTPCSeats[i]["OBC"]["F"], 0);
    }
    Iterate();
    for(int i=0; i<3; i++){
        NTPCSeats[i]["UR"]["OPEN"]+= exchange(NTPCSeats[i]["EWS"]["OPEN"], 0);
        NTPCSeats[i]["UR"]["F"]+= exchange(NTPCSeats[i]["EWS"]["F"], 0);
    }
    Iterate();
}

void WriteAllotmentsToCSV(){
    ofstream fout(AllotmentsFilePath);

    if (!fout.is_open()) {
        cout << "Error opening file: " << AllotmentsFilePath << endl;
        return;
    }

    // Hardcoded CSV header
    fout << "S No.,Application Number,Name,EmailId,Rank,Branch,Allotted Category\n";

    // Write each row
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
    if( !ExtractingDataFromCSV() ) return 0;
    if( !ExtractingSeatMatrixCSV() ) return 0;
    
    // for(Applicant &x: Applicants) cout <<x.Rank <<", ";
    // cout <<endl;
    MainLogic();
    // displaySeatMatrix();
    
    MergeAllotments();
    WriteAllotmentsToCSV();
    return 0;
}