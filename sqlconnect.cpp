/*
* C++/Cli �M ADO.NET�{��
* 
* ��: �]���O��.NET�����Ҩӳs����Ʈw��s�J���
*     �ҥH�̦n�ɶq��.NET�����O�ӹ�@
*/

/* *** C++�зǨ禡�w *** */
#include <comdef.h>
#include <gcroot.h>
#include <iostream>
#include <string>
#include <random>

using namespace std;
/*************************/

/* ************* �ϥ�.NET���禡�w ************** */
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

// �����ܼ�
int index = 0;
static string host = "140.128.109.115";
static string user = "sa";
static string password = "s08490043";
static string database = "CppTest";


/* ********************** �������������O ********************** */
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
    // ���������ä���(uniform distrbution)�ҫ�
    int nextInt(int max = RAND_MAX)
    {
        return uniform_int_distribution<int>(0, max)(engine);
    }

    // �s�򫬧��ä���(uniform distrbution)�ҫ�
    double nextDouble(double max = (double)RAND_MAX)
    {
        return uniform_real_distribution<double>(0, max)(engine);
    }

    // �зǱ`�A����(normal distrbution)�ҫ�
    double nextGuassian()
    {
        return normal_distribution<double>()(engine);
    }

    // ���Ƥ���(exponential distrbution)�ҫ�
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


/* ��Ʈw�ާ@ */
void createDB(String^ table_name);
void createTable(String^ table_name);
void insertData(String^ table_name);
void deleteData(String^ table_name);

/* �p���ƪ��ƪ����� */
int countData();

/* ���͸�� */
HeavyMachine^ dataGenerator();

/* �N.NET�̪�Dictionary�ഫ���r�� */
String^ dictToString(Dictionary<String^, String^ >^ dict);

/* �D�{�� */
int main(){

    DateTime start = DateTime::Now;
    DateTime today = DateTime::Today;

    Console::WriteLine("���: " + today.ToString("yyyy-MM-dd"));

    // �۰ʫإ߸�Ʈw(�Y���h���L)
    createDB("CppTest");

    // �۰ʫإ߸�ƪ�(�Y���h���L)
    createTable("HeavyMachine");

    // �������x�]���
    while (true)
    {
        DateTime end = DateTime::Now;
        
        
        if ((end - start).Minutes == 30)   break;

        insertData("HeavyMachine");
        
        Sleep(300);
    }

    // �{������
    Console::WriteLine("----------�{������----------");
    Console::Write("�Ы��UEnter�~��...... ");
    Console::ReadKey();
    
    return 0;
}

int countData()
{
    int num = 0;

    try
    {
        /* �إߤ@�ӳs����Ʈw������ */
        /*      �öǤJ�s�u�r��      */
        SqlConnection^ conn = gcnew SqlConnection(
            String::Format("Server={0};UID={1};PWD={2};\
            TrustServerCertificate=true;\
            database={3}", gcnew String(host.c_str())
                         , gcnew String(user.c_str())
                         , gcnew String(password.c_str())
                         , gcnew String(database.c_str())
            ));

        // �s����Ʈw
        conn->Open();

        // �p���Ƽƥت�SQL�r��
        String^ str = "SELECT COUNT(*) FROM HeavyMachine";
        SqlCommand^ cmd = gcnew SqlCommand(str, conn);
        
        // �N��Ƶ��ƶǦ^num
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
        Console::WriteLine("��ƧR������><");

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
        Console::WriteLine("��Ʒs�W����><");
        
        MessageBox::Show(e->ToString(), "��ƽĬ�θ�ƪ��s�b",
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

        Console::WriteLine("��ƪ�إߥ���><");

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

            // �p�G��ƪ��ƬO�Ū��ܴN���J���
            if (((int)cmd->ExecuteScalar()) == 0)
                myCommand->ExecuteNonQuery();

            if (conn->State == ConnectionState::Open)
            {
                conn->Close();
            }
        }
        catch (Exception^ e)
        {

            Console::WriteLine("��Ʈw�إߥ���><");

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

    
    // .NET�ۤv���r�嫬�O
    Dictionary<String^, String^>^ dict = gcnew Dictionary<String^, String^>();
    
    /* ********************** �إ߸�ƥD�� ************************ */
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
    
    // �ഫ�� C++/Cli�̪��r�ꫬ�A
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

    // �]���b.NET�̨S���NDictionary�ন
    // �ҭn�榡���r�ꪺ��k
    // �ҥH�ۤv�ⰵ�@��
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

    // �Ǧ^�榡�r��
    return left_bracket + format + right_bracket;
    
}