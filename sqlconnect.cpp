/*
* C++/Cli 和 ADO.NET程式
* 
* 註: 因為是用.NET的環境來連接資料庫跟存入資料
*     所以最好盡量用.NET的型別來實作
*/

/* *** C++標準函式庫 *** */
#include <comdef.h>
#include <gcroot.h>
#include <iostream>
#include <string>
#include <random>

using namespace std;
/*************************/

/* ************* 使用.NET的函式庫 ************** */
#using <mscorlib.dll>
using namespace System;

#using <system.dll>
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::Runtime::InteropServices;

#using <System.data.dll>
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::Runtime::InteropServices;

#using <system.windows.forms.dll>
using namespace System::Windows::Forms;
/* ********************************************* */

// 全域變數
int index = 0;
static string host = "140.128.109.115";
static string user = "sa";
static string password = "s08490043";
static string database = "CppTest";


/* ********************** 模擬機器的類別 ********************** */
ref class HeavyMachine {
public:
    String^ DeviceId;
    String^ DeviceValue;
    DateTime UpdateTime;

    
    HeavyMachine() {}
    HeavyMachine(String^ id, String^ value, DateTime datetime) 
    {
        this->DeviceId = id;
        this->DeviceValue = value;
        this->UpdateTime = datetime;
    }
};
/****************************************************************/


class RandomModel {
private:
    default_random_engine engine{ random_device{}() };
public:
    // 離散型均勻分布(uniform distrbution)模型
    int nextInt(int max = RAND_MAX)
    {
        return uniform_int_distribution<int>(0, max)(engine);
    }

    // 連續型均勻分布(uniform distrbution)模型
    double nextDouble(double max = (double)RAND_MAX)
    {
        return uniform_real_distribution<double>(0, max)(engine);
    }

    // 標準常態分布(normal distrbution)模型
    double nextGuassian()
    {
        return normal_distribution<double>()(engine);
    }

    // 指數分布(exponential distrbution)模型
    double nextPoisson()
    {
        return exponential_distribution<double>()(engine);
    }

    int nextBinomial()
    {
        return binomial_distribution<int>()(engine);
    }

    auto nextRandom(String^ mode)
    {   
        
    }

    RandomModel() = default;

};


/* 資料庫操作 */
void createDB(String^ table_name);
void createTable(String^ table_name);
void insertData(String^ table_name);
void deleteData(String^ table_name);

/* 計算資料表資料的筆數 */
int countData();

/* 產生資料 */
HeavyMachine^ dataGenerator();

/* 將.NET裡的Dictionary轉換成字串 */
String^ dictToString(Dictionary<String^, String^ >^ dict);

/* 主程式 */
int main(){

    DateTime start = DateTime::Now;
    DateTime today = DateTime::Today;

    Console::WriteLine("日期: " + today.ToString("yyyy-MM-dd"));

    // 自動建立資料庫(若有則跳過)
    createDB("CppTest");

    // 自動建立資料表(若有則跳過)
    createTable("HeavyMachine");

    // 模擬機台跑資料
    while (true)
    {
        DateTime end = DateTime::Now;
        
        
        if ((end - start).Minutes == 30)   break;

        insertData("HeavyMachine");
        
        Sleep(300);
    }

    // 程式結束
    Console::WriteLine("----------程式結束----------");
    Console::Write("請按下Enter繼續...... ");
    Console::ReadKey();
    
    return 0;
}

int countData()
{
    int num = 0;

    try
    {
        /* 建立一個連接資料庫的物件 */
        /*      並傳入連線字串      */
        SqlConnection^ conn = gcnew SqlConnection(
            String::Format("Server={0};UID={1};PWD={2};\
            TrustServerCertificate=true;\
            database={3}", gcnew String(host.c_str())
                         , gcnew String(user.c_str())
                         , gcnew String(password.c_str())
                         , gcnew String(database.c_str())
            ));

        // 連接資料庫
        conn->Open();

        // 計算資料數目的SQL字串
        String^ str = "SELECT COUNT(*) FROM HeavyMachine";
        SqlCommand^ cmd = gcnew SqlCommand(str, conn);
        
        // 將資料筆數傳回num
        num = (int)cmd->ExecuteScalar();
        
        if (conn->State == ConnectionState::Open)
        {
            conn->Close();
        }

    }
    catch (Exception^ e)
    {
        MessageBox::Show(e->ToString(), "HeavyMachine",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
    }

    return num;
}


void deleteData(String^ table_name)
{
    try
    {
        SqlConnection^ conn = gcnew SqlConnection(
            String::Format("Server={0};UID={1};PWD={2};\
            TrustServerCertificate=true;\
            database={3}", gcnew String(host.c_str())
                , gcnew String(user.c_str())
                , gcnew String(password.c_str())
                , gcnew String(database.c_str())
            ));

        conn->Open();

        DateTime today = DateTime::Today;

        String^ three_days_ago = "DELETE FROM " + table_name + 
            " WHERE (SELECT * FROM "+ table_name + " WHERE UpdateTime > " + 
            today.ToString() + " ); ";
        
        String^ str = "WITH FIRST AS( SELECT TOP 1 * FROM " + table_name +
            " ORDER BY DeviceId ASC) DELETE FIRST";

        SqlCommand^ cmdDelete = gcnew SqlCommand(three_days_ago, conn);

        Console::WriteLine(str);

        cmdDelete->ExecuteNonQuery();

        if (conn->State == ConnectionState::Open)
        {
            conn->Close();
        }
    }
    catch (Exception^ e)
    {
        Console::WriteLine("資料刪除失敗><");

        MessageBox::Show(e->ToString(), "HeavyMachine",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
    }
}

void insertData(String^ table_name) {

    try
    {
        HeavyMachine^ machine = dataGenerator();

        SqlConnection^ conn =  gcnew SqlConnection(
            String::Format("Server={0};UID={1};PWD={2};\
            TrustServerCertificate=true;\
            database={3}", gcnew String(host.c_str())
                , gcnew String(user.c_str())
                , gcnew String(password.c_str())
                , gcnew String(database.c_str())
            ));
        
        conn->Open();

        String^ str = String::Format("INSERT INTO {0} VALUES(\'{1}\', \'{2}\', \'{3}\');",
            table_name, machine->DeviceId ,machine->DeviceValue, 
            machine->UpdateTime.ToString("yyyy-MM-dd HH:mm:ss:fff"));


        Console::WriteLine(str);

        SqlCommand^ cmdInsert = gcnew SqlCommand(str, conn);

        cmdInsert->ExecuteNonQuery();

        if (conn->State == ConnectionState::Open)
        {
            conn->Close();
        }
    }
    catch (Exception^ e)
    {
        Console::WriteLine("資料新增失敗><");
        
        MessageBox::Show(e->ToString(), "資料衝突或資料表不存在",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
    }
    
}


void createTable(String^ table_name)
{
    try {
        SqlConnection^ conn = gcnew SqlConnection(
            String::Format("Server={0};UID={1};PWD={2};\
            TrustServerCertificate=true;\
            database={3}", gcnew String(host.c_str())
                , gcnew String(user.c_str())
                , gcnew String(password.c_str())
                , gcnew String(database.c_str())
            ));
        
        conn->Open();
        
        String^ table_sql = "SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME=\'HeavyMachine\';";
        
        SqlCommand^ cmd = gcnew SqlCommand(table_sql, conn);
        
        if (((int)cmd->ExecuteScalar()) == 0)
        {
            String^ sql_str = "CREATE TABLE " + table_name +
                "(DeviceId        nvarchar(6) NOT NULL, \
                    DeviceValue     nvarchar(MAX) NULL, \
                    UpdateTime      datetime NOT NULL);";

            SqlCommand^ command = gcnew SqlCommand(sql_str, conn);

            command->ExecuteNonQuery();
        }
       
        if (conn->State == ConnectionState::Open)
        {
            conn->Close();
        }
    }
    catch (Exception^ e) {

        Console::WriteLine("資料表建立失敗><");

        MessageBox::Show(e->ToString(), "HeavyMachine",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);

    }
}

void createDB(String^ dbName)
{
    
    SqlConnection^ conn =  gcnew SqlConnection(
        String::Format("Server={0};UID={1};PWD={2};\
            TrustServerCertificate=true;"
            , gcnew String(host.c_str())
            , gcnew String(user.c_str())
            , gcnew String(password.c_str())
        ));
    
    conn->Open();



    Console::WriteLine("Press 'C' and then ENTER to create a new database");
    Console::WriteLine("Press any other key and then ENTER to quit");
    char c = Console::Read();

    if (c == 'C' || c == 'c')
    {
        
        Console::WriteLine("Creating the database...");
        String^ str;
        
        str = "CREATE database " + dbName + ";";
        
        try
        {
            SqlCommand^ myCommand = gcnew SqlCommand(str, conn);
            
            String^ table_sql = "SELECT COUNT(*) FROM SYSDATABASES WHERE name=\'CppTest\';";
            SqlCommand^ cmd = gcnew SqlCommand(table_sql, conn);

            // 如果資料表資料是空的話就插入資料
            if (((int)cmd->ExecuteScalar()) == 0)
                myCommand->ExecuteNonQuery();

            if (conn->State == ConnectionState::Open)
            {
                conn->Close();
            }
        }
        catch (Exception^ e)
        {

            Console::WriteLine("資料庫建立失敗><");

            MessageBox::Show(e->ToString(), "HeavyMachine",
                MessageBoxButtons::OK,
                MessageBoxIcon::Information);
        }
    }
}


HeavyMachine^ dataGenerator() {
    
    HeavyMachine^ data;
    Random^ rand = gcnew Random();
    DateTime date;

    RandomModel model =  RandomModel();

    
    // .NET自己的字典型別
    Dictionary<String^, String^>^ dict = gcnew Dictionary<String^, String^>();
    
    /* ********************** 建立資料主區 ************************ */
    dict["DeviceTypeId"] = "0";
    dict["StatusId"] = "0" ;
    dict["ActStatus"] = "ON" ;
    dict["ActAlarmMsg"] = "[15][1001] EMERGENCY STOP" ;
    dict["ActMainProgramName"] = "O602" ;
    dict["ActMainProgramNote"] = "" ;
    dict["ActPartCount"] =  rand->Next(10).ToString();
    dict["Profit"] = (rand->NextDouble() * (10+1)).ToString("F0");
    dict["ProductionRequiredQty"] = rand->Next(10).ToString();
    dict["ProductionRequiredQtyTotal"] = rand->Next(10).ToString();
    /****************************************************************/
    
    // 轉換成 C++/Cli裡的字串型態
    String^ json_str = dictToString(dict);
    
    
    Console::WriteLine("DeviceValue: " + json_str);
    
    data = gcnew HeavyMachine("hm0001", json_str, DateTime::Now);

    return data;
}

String^ dictToString(Dictionary<String^, String^>^ dict)
{
    String^ left_bracket = "{";
    String^ right_bracket = "}";
    String^ format = "";

    // 因為在.NET裡沒有將Dictionary轉成
    // 所要格式的字串的方法
    // 所以自己手做一個
    format += String::Format(
        "\"{0}\": {1}," +
        "\"{2}\": {3}," +
        "\"{4}\": \"{5}\"," +
        "\"{6}\": \"{7}\"," +
        "\"{8}\": \"{9}\"," +
        "\"{10}\": \"{11}\"," +
        "\"{12}\": {13}," +
        "\"{14}\": {15: F1}," +
        "\"{16}\": {17}," +
        "\"{18}\": {19}", 
        "DeviceTypeId", dict["DeviceTypeId"],
        "StatusId", dict["StatusId"],
        "ActStatus", dict["ActStatus"],
        "ActAlarmMsg", dict["ActAlarmMsg"],
        "ActMainProgramName", dict["ActMainProgramName"],
        "ActMainProgramNote", dict["ActMainProgramNote"],
        "ActPartCount", dict["ActPartCount"],
        "Profit", dict["Profit"],
        "ProductionRequiredQty", dict["ProductionRequiredQty"],
        "ProductionRequiredQtyTotal", dict["ProductionRequiredQtyTotal"]
    );

    // 傳回格式字串
    return left_bracket + format + right_bracket;
    
}