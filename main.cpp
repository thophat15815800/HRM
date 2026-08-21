#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include "database.h"

using namespace std;

int main() {
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    SQLCHAR outstr[1024];
    SQLSMALLINT outstrlen;
    ret = SQLDriverConnect(dbc, NULL,
        (SQLCHAR*)"DRIVER={ODBC Driver 17 for SQL Server};SERVER=localhost;DATABASE=QuanLyNhanSu;Trusted_Connection=yes;TrustServerCertificate=yes;",
        SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_NOPROMPT);
    if (SQL_SUCCEEDED(ret)) {
        cout << "Ket noi Database thanh cong!" << endl;
    } else {
        cout << "Ket noi that bai!" << endl;
        return -1;
    }
    string maTaiKhoan, matKhau;
    cout << "\n--- DANG NHAP HE THONG ---" << endl;
    cout << "Nhap ma tai khoan: "; cin >> maTaiKhoan;
    cout << "Nhap mat khau: "; cin >> matKhau;
    if (kiemTraDangNhap(dbc, maTaiKhoan, matKhau)) {
        cout << "Dang nhap thanh cong!" << endl;
        string phanQuyen = layPhanQuyen(dbc, maTaiKhoan);
        int luaChon;
        do {
            if (phanQuyen == "Admin" || phanQuyen == "Manager") {
                cout << "\n================ MENU ===============" << endl;
                cout << "1. Xem danh sach nhan vien" << endl;
                cout << "2. Tim kiem nhan vien" << endl;
                cout << "3. Them nhan vien" << endl;
                cout << "4. Sua thong tin nhan vien" << endl;
                cout << "5. Xoa nhan vien" << endl;
                cout << "0. Dang xuat" << endl;
                cout << "=====================================" << endl;
                cout << "Chon chuc nang: ";
                cin >> luaChon;

                switch (luaChon) {
                    case 1: hienThiDanhSachNhanVien(dbc); break;
                    case 2: timKiemNhanVien(dbc); break;
                    case 3: themNhanVien(dbc); break;
                    case 4: suaNhanVien(dbc, maTaiKhoan); break;
                    case 5: xoaNhanVien(dbc); break;
                    case 0: cout << "Dang xuat tai khoan..." << endl; break;
                    default: cout << "Lua chon khong hop le!" << endl;
                }
            } else {
                cout << "\n================ MENU ===============" << endl;
                cout << "1. Xem danh sach nhan vien" << endl;
                cout << "2. Tim kiem nhan vien" << endl;
                cout << "3. Sua thong tin ca nhan" << endl;
                cout << "0. Dang xuat" << endl;
                cout << "=====================================" << endl;
                cout << "Chon chuc nang: ";
                cin >> luaChon;

                switch (luaChon) {
                    case 1: hienThiDanhSachNhanVien(dbc); break;
                    case 2: timKiemNhanVien(dbc); break;
                    case 3: suaNhanVien(dbc, maTaiKhoan); break;
                    case 0: cout << "Dang xuat tai khoan..." << endl; break;
                    default: cout << "Lua chon khong hop le!" << endl;
                }
            }
        } while (luaChon != 0);
    } else {
        cout << "Dang nhap that bai! Sai tai khoan hoac mat khau." << endl;
    }
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);
    return 0;
}
