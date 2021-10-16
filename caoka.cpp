#define DATA_NAME "1_attlog.dat"
#define STUDENTS_LIST "students.txt"

#define FROM_TIME "07:10:00"
#define TO_TIME "07:20:00"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <filesystem>
#include <windows.h>
using namespace std;

set<string> ids;
map<string, string> studentMap;
map<string, bool> signMap;
int from, to;

bool Check()
{
    if (!filesystem::exists(DATA_NAME))
    {
        MessageBox(NULL, L"操卡机数据不存在！\n请将操卡机储存中的 1_attlog.dat 数据文件复制到程序当前目录", L"错误", MB_OK | MB_ICONERROR);
        return false;
    }
    else if (!filesystem::exists(STUDENTS_LIST))
    {
        MessageBox(NULL, L"学生名单数据不存在！\n请将学生 姓名-打卡id 对应的数据填入同目录的 students.txt 文件中", L"错误", MB_OK | MB_ICONERROR);
        ofstream fout(STUDENTS_LIST);
        fout.close();
        return false;
    }
    return true;
}

void Welcome()
{
    cout << endl;
    cout << "        ==========================================================" << endl;
    cout << "                              2020级 操卡统计" << endl;
    cout << "                               by yqs112358" << endl;
    cout << "        ==========================================================" << endl;
    cout << "                开源地址：https://github.com/yqs112358/caoka" << endl << endl;
}

void ReadStudents()
{
    cout << "[Info] 正在读取学生名单..." << endl;
    ifstream fin(STUDENTS_LIST);
    string id, name;
    int cnt = 0;
    while (fin >> name)
    {
        fin >> id;
        studentMap[id] = name;
        ids.insert(id);
        ++cnt;
    }
    cout << "[Info] 学生名单读取完毕。已建立" << cnt << "条映射" << endl;
}

bool CheckFormat(string date)
{
    if (date.size() != 8)
        return false;
    for (auto& c : date)
        if (c < '0' || c > '9')
            return false;
    return true;
}

void GetDateRange()
{
    string temp;
    while (true)
    {
        cout << endl << "====================================================================" << endl;
        cout << "[Info] 请输入要统计操卡的 >起始< 年月日（如 20200802），输入完毕后请按回车：";
        cin >> temp;
        if (!CheckFormat(temp))
        {
            cout << "[ERROR] 输入日期格式错误！请重新输入" << endl << endl;
            continue;
        }
        from = stoi(temp);
        break;
    }

    while (true)
    {
        cout << "[Info] 请输入要统计操卡的 >终止< 年月日（如 20200808），输入完毕后请按回车：";
        cin >> temp;
        if (!CheckFormat(temp))
        {
            cout << "[ERROR] 输入日期格式错误！请重新输入" << endl << endl;
            continue;
        }
        to = stoi(temp);
        break;
    }

    cout << endl;
    cout << "[Info] 统计开始日期：" << from << " \t统计结束日期：" << to << endl;
    Sleep(1000);
}

void PreProcess()
{
    for (int i = from; i <= to; ++i)
    {
        for (auto& [id, name] : studentMap)
            signMap[to_string(i) + " " + id] = false;
    }
}

string ProcessDateStr(int dat)
{
    string date;
    try
    {
        date = to_string(dat);
    }
    catch (...)
    {
        return "";
    }

    date.insert(4, 1, '-');
    date.insert(7, 1, '-');
    return date;
}

void ProcessData()
{
    string lastDate = "";
    cout << "[Info] 开始统计..." << endl;

    ifstream fin(DATA_NAME);
    string line, id, date, time;
    while (getline(fin, line))
    {
        if (line.empty())
            break;
        istringstream sin(line);
        sin >> id >> date >> time;
        if (ids.count(id) == 0)
            continue;

        if (date != lastDate)
        {
            lastDate = date;
            cout << "[Info] 正在处理 " << lastDate << " 的数据" << endl;
        }

        //Check
        if (time < FROM_TIME || time > TO_TIME)
            continue;
        date.erase(4, 1);
        date.erase(6, 1);
        signMap[date + " " + id] = true;
    }
    cout << "[Info] 处理完毕" << endl << endl;

    for (int i = from; i <= to; ++i)
    {
        int cnt = 0;
        cout << "[Info] " << ProcessDateStr(i) << " 未打卡名单：" << endl;
        for (auto& [id, name] : studentMap)
            if (!signMap[to_string(i) + " " + id])
            {
                if (++cnt == 10)
                {
                    cout << endl;
                    cnt = 0;
                }
                cout << studentMap[id] << "\t";
            }
        cout << endl << endl;
    }
    cout << "[Info] 统计已完成" << endl;
    cout << "[Info] 提示：对于查出未打操卡的同学，请务必与其取得联系，确认是否因为使用免操卡，或者有其他特殊情况导致记录缺失" << endl;
}

int main()
{
    if (!Check())
        return 1;
    Welcome();
    ReadStudents();
    GetDateRange();
    PreProcess();
    ProcessData();
    system("pause > nul");
    return 0;
}