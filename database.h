#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>

bool kiemTraDangNhap(SQLHDBC dbc, std::string maTaiKhoanNhap, std::string matKhauNhap);
std::string layPhanQuyen(SQLHDBC dbc, std::string maTK);
void hienThiDanhSachNhanVien(SQLHDBC dbc);
void themNhanVien(SQLHDBC dbc);
void suaNhanVien(SQLHDBC dbc, std::string maTaiKhoanDangNhap);
void xoaNhanVien(SQLHDBC dbc);
void timKiemNhanVien(SQLHDBC dbc);

#endif
