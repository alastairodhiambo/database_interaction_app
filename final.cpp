/*
Filename:   final.cpp
Name:       Alastair Odhiambo
Date:       August 11, 2020
*/

 

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <occi.h>


using oracle::occi::Environment;
using oracle::occi::Connection;

using namespace oracle::occi;
using namespace std;

const int MIN = 1;
const int MAX = 6;

struct Employee {
	int employeeNumber;
	char lastName[50];
	char firstName[50];
	char email[100];
	char phone[50];
	char extension[10];
	char reportsTo[100];
	char jobTitle[50];
	char city[50];
};

int receiveInt();

int menu(void);

int findEmployee(Connection *conn, int employeeid, Employee* emp);

void displayEmployee(Connection *conn, Employee*);

void displayAllEmployees(Connection *conn);

void insertEmployee(Connection* conn, Employee* emp);

void updateEmployee(Connection* conn, int employeeNumber);

void deleteEmployee(Connection* conn, int employeeNumber);

bool yesNo(std::istream& in = cin, std::ostream& os = cout);


int main(void)
{
    Environment* env = nullptr;
    Connection* conn = nullptr;
    Statement* stmt = nullptr;
    ResultSet* rs = nullptr;

    string user = "d########";
    string pass = "1********";
    string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";

    Employee* emp = new Employee;
    *emp = { 0, "", "", "", "", "", "", ""};

    try {

        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection(user, pass, constr);

        int empNo = 0;
        int check = 0;

        do {
            switch (menu()) {
            case 1:
                empNo = receiveInt();
                if (findEmployee(conn, empNo, emp)) {
                    displayEmployee(conn, emp);
                }
                else cout << "Employee " << empNo << " does not exist." << endl;
                break;
            case 2:
                displayAllEmployees(conn);
                break;
            case 3:
                insertEmployee(conn, emp);
                break;

            case 4:
                updateEmployee(conn, receiveInt());
                break;

            case 5:
                deleteEmployee(conn, receiveInt());
                break;

            default:
                cout << "Are you sure you want to exit? All statements will be committed." << endl;
                cout << "Enter y or n: ";
                if (yesNo()) {
                    Statement* stmt = conn->createStatement();
                    stmt->execute("COMMIT");
                    check = 1;
                }
            }
        } while (check == 0);

        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);

    }
    catch (SQLException& sqlExcp) {
        cout << "error";
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }

    delete emp;
    emp = nullptr;

    return 0;
}

int menu(void)
{
    int option = 0;

    cout << "********************* HR Menu *********************" << endl;
    cout << "1) Find Employee" << endl;
    cout << "2) Employees Report" << endl;
    cout << "3) Add Employee" << endl;
    cout << "4) Update Employee" << endl;
    cout << "5) Remove Employee" << endl;
    cout << "6) Exit" << endl;
    cout << "Enter an option: ";

    do {
        cin >> option;
        cin.ignore(2000, '\n');

        if (cin.fail() || option == 0) {
            cout << "Invalid Integer, try again: ";
            cin.clear();
        }
        else if (option < MIN || option > MAX) {
            cout << "Invalid selection, try again: ";
        }
    } while (!(option >= MIN && option <= MAX));

    return option;
}



//Find the Employee:
int findEmployee(Connection *conn, int employeeid, Employee* emp)
{
    int check = 0;

    string empNo = to_string(employeeid);

    Statement*stmt = conn->createStatement();
    ResultSet* rs = stmt->executeQuery("SELECT e.employeenumber, e.firstname, e.lastname, e.email, o.phone, e.extension, r.firstname || ' ' || r.lastname AS REPORTS, e.jobtitle, o.city FROM employees e JOIN offices o ON e.officecode = o.officecode AND e.employeenumber = " + empNo + " LEFT OUTER JOIN employees r ON e.reportsto = r.employeenumber");

    while (rs->next())
    {
        if (employeeid == rs->getInt(1)) {
            emp->employeeNumber = rs->getInt(1);
            std::strcpy(emp->firstName, rs->getString(2).c_str());
            std::strcpy(emp->lastName, rs->getString(3).c_str());
            std::strcpy(emp->email, rs->getString(4).c_str());
            std::strcpy(emp->phone, rs->getString(5).c_str());
            std::strcpy(emp->extension, rs->getString(6).c_str());
            std::strcpy(emp->reportsTo, rs->getString(7).c_str());
            std::strcpy(emp->jobTitle, rs->getString(8).c_str());
            std::strcpy(emp->city, rs->getString(9).c_str());

            check = 1;
        }
    }

    return check;
}

//receiveInt

int receiveInt()
{
    int empNo = 0;
    cout << "Enter employee number: ";
    cin >> empNo;
    cin.ignore(2000, '\n');
    cout << endl;

    return empNo;
}

//Display Employee:
void displayEmployee(Connection *conn, Employee* emp)
{
    cout << "employeeNumber = " << emp->employeeNumber << endl;
    cout << "lastName = " << emp->lastName << endl;
    cout << "firstName = " << emp->firstName << endl;
    cout << "email = " << emp->email << endl;
    cout << "phone = " << emp->phone << endl;
    cout << "extension = " << emp->extension << endl;
    cout << "reportsTo = " << emp->reportsTo << endl;
    cout << "jobTitle = " << emp->jobTitle << endl;
    cout << "city = " << emp->city << endl << endl;
}

void displayAllEmployees(Connection* conn)
{

    Statement* stmt = conn->createStatement();
    ResultSet* rs = stmt->executeQuery("SELECT e.employeenumber, e.firstname, e.lastname, e.email, o.phone, e.extension, r.firstname || ' ' || r.lastname AS REPORTS, e.jobtitle, o.city FROM employees e JOIN offices o ON e.officecode = o.officecode LEFT OUTER JOIN employees r ON e.reportsto = r.employeenumber ORDER BY e.employeenumber");

        cout.setf(ios::left);
        cout.width(10);
        cout << "E";
        cout.width(20);
        cout << "Employee Name";
        cout.width(40);
        cout << "Email";
        cout.width(20);
        cout << "Phone";
        cout.width(10);
        cout << "Ext";
        cout << "Manager" << endl;
        cout << "----------------------------------------------------------------------------------------------------------------------" << endl;

        while (rs->next())
        {
            cout.width(10);
            cout << rs->getInt(1);
            string name = rs->getString(2) + " " + rs->getString(3);
            cout.width(20);
            cout << name;
            cout.width(40);
            cout << rs->getString(4);
            cout.width(20);
            cout << rs->getString(5);
            cout.width(10);
            cout << rs->getString(6);
            cout << rs->getString(7) << endl;
        }

        cout << endl;
        cout.unsetf(ios::left);
}


void insertEmployee(Connection* conn, Employee* emp)
{
    int empNo = receiveInt();

    if (findEmployee(conn, empNo, emp)) {
        cout << "An employee with the same number already exists." << endl;
    }
    else {
        string empNoStr = to_string(empNo);

        emp->employeeNumber = empNo;
        cout << "Employee Number: " << emp->employeeNumber << endl;
        cout << "Last Name: ";
        cin.getline(emp->lastName, 49);
        cout << "First Name: ";
        cin.getline(emp->firstName, 49);
        cout << "Email: ";
        cin.getline(emp->email, 99);
        cout << "extension: ";
        cin.getline(emp->extension, 9);
        cout << "Job Title: ";
        cin.getline(emp->jobTitle, 49);
        cout << "City: ";
        cin.getline(emp->city, 49);
        string reportsTo = "1002";
        char officecode = '8';

        Statement* stmt = conn->createStatement();
        stmt->execute("INSERT INTO employees values (" + empNoStr + ", \'" + emp->lastName + "\', \'" + emp->firstName + "\', \'" + emp->extension + "\', \'" + emp->email + "\', " + officecode + ", \'" + reportsTo + "\', \'" + emp->jobTitle + "\')");
    }
}

void updateEmployee(Connection* conn, int employeeNumber)
{
    Employee* emp = new Employee;
    *emp = { 0, "", "", "", "", "", "", "" };

    if (findEmployee(conn, employeeNumber, emp)) {
       string empNoStr = to_string(employeeNumber);
       string newExt = "";
       cout << "New Extension: ";
       getline(cin, newExt);
       Statement* stmt = conn->createStatement();
       stmt->executeUpdate("UPDATE employees SET extension = \'" + newExt + "\' WHERE employeenumber = " + empNoStr);
    }
    else cout << "Employee " << employeeNumber << " does not exist." << endl;
    
    delete emp;
    
}

void deleteEmployee(Connection* conn, int employeeNumber)
{
    Employee* emp = new Employee;
    *emp = { 0, "", "", "", "", "", "", "" };

    if (findEmployee(conn, employeeNumber, emp)) {
        string empNoStr = to_string(employeeNumber);
        Statement* stmt = conn->createStatement();
        stmt->execute("DELETE FROM employees WHERE employeenumber = " + empNoStr);
    }
    else cout << "Employee " << employeeNumber << " does not exist." << endl;

    delete emp;
}


//Y or N confirmation
bool yesNo(std::istream& in, std::ostream& os) {

    std::string str;

    bool value = false;
    int flag = 1;
    do {
        in >> str;
        in.ignore(2000, '\n');

        if (strcmp("Y", str.c_str()) == 0 || strcmp("y", str.c_str()) == 0) {
            value = true;
            flag = 0;
        }
        else if (strcmp("N", str.c_str()) == 0 || strcmp("n", str.c_str()) == 0) {
            value = false;
            flag = 0;
        }
        else {
            os << "Invalid response, only (Y/y) or (N/n) are acceptable, retry: ";
        }
    } while (flag != 0);

    return value;
}
