#include "database.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>

using namespace std;

string layPhanQuyen(SQLHDBC dbc, string maTK) {
    SQLHSTMT hstmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
    string query = "SELECT PhanQuyen FROM TaiKhoan WHERE MaTaiKhoan = '" + maTK + "'";
    SQLExecDirect(hstmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    SQLCHAR pq[50];
    string phanQuyen = "Employee";
    if (SQLFetch(hstmt) == SQL_SUCCESS) {
        SQLGetData(hstmt, 1, SQL_C_CHAR, pq, sizeof(pq), NULL);
        phanQuyen = string((char*)pq);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return phanQuyen;
}

bool kiemTraDangNhap(SQLHDBC dbc, string maTaiKhoanNhap, string matKhauNhap) {
    SQLHSTMT hstmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);

    // So sánh mật khẩu đã được băm
    string query = "SELECT MaTaiKhoan FROM TaiKhoan "
                   "WHERE MaTaiKhoan = '" + maTaiKhoanNhap + "' "
                   "AND MatKhau = CONVERT(VARCHAR(64), HASHBYTES('SHA2_256', '" + matKhauNhap + "'), 2)";
    SQLExecDirect(hstmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    bool ketQua = false;
    if (SQLFetch(hstmt) == SQL_SUCCESS) {
        ketQua = true;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return ketQua;
}

void hienThiDanhSachNhanVien(SQLHDBC dbc) {
    SQLHSTMT hstmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
    string query = "SELECT n.MaNhanVien, n.HoTen, n.Email, p.TenPhongBan, n.GioiTinh "
                   "FROM NhanVien n "
                   "LEFT JOIN PhongBan p ON n.MaPhongBan = p.MaPhongBan";
    SQLExecDirect(hstmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    SQLCHAR maNV[20], hoTen[100], email[100], tenPhong[100], gioiTinh[10];
    SQLLEN cbMaNV, cbHoTen, cbEmail, cbTenPhong, cbGioiTinh;

    cout << "\n--- DANH SACH NHAN VIEN ---" << endl;
    cout << left << setw(10) << "Ma NV" << " | "
         << setw(20) << "Ho va Ten" << " | "
         << setw(15) << "Gioi Tinh" << " | "
         << setw(25) << "Email" << " | "
         << "Phong Ban" << endl;
    cout << string(95, '-') << endl;

    while (SQLFetch(hstmt) == SQL_SUCCESS) {
        SQLGetData(hstmt, 1, SQL_C_CHAR, maNV, sizeof(maNV), &cbMaNV);
        SQLGetData(hstmt, 2, SQL_C_CHAR, hoTen, sizeof(hoTen), &cbHoTen);
        SQLGetData(hstmt, 3, SQL_C_CHAR, email, sizeof(email), &cbEmail);
        SQLGetData(hstmt, 4, SQL_C_CHAR, tenPhong, sizeof(tenPhong), &cbTenPhong);
        SQLGetData(hstmt, 5, SQL_C_CHAR, gioiTinh, sizeof(gioiTinh), &cbGioiTinh);

        string strMaNV((char*)maNV), strHoTen((char*)hoTen);
        string strEmail = (cbEmail == SQL_NULL_DATA) ? "N/A" : string((char*)email);
        string strTenPhong = (cbTenPhong == SQL_NULL_DATA) ? "Chua phan ban" : string((char*)tenPhong);
        string strGioiTinh = (cbGioiTinh == SQL_NULL_DATA) ? "Chua ro" : string((char*)gioiTinh);

        cout << left << setw(10) << strMaNV << " | "
             << setw(20) << strHoTen << " | "
             << setw(15) << strGioiTinh << " | "
             << setw(25) << strEmail << " | "
             << strTenPhong << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
}

void themNhanVien(SQLHDBC dbc) {
    string maNV, hoTen, maTK, email, gt;
    cout << "\n--- THEM NHAN VIEN MOI ---" << endl;
    cout << "Nhap ma nhan vien: "; cin >> maNV;
    cout << "Nhap ho ten: "; cin.ignore(); getline(cin, hoTen);
    cout << "Nhap ma tai khoan: "; cin >> maTK;
    cout << "Nhap email: "; cin >> email;
    cout << "Nhap gioi tinh: "; cin >> gt;

    SQLHSTMT hstmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
    string checkNV = "SELECT MaNhanVien FROM NhanVien WHERE MaNhanVien = '" + maNV + "'";
    SQLExecDirect(hstmt, (SQLCHAR*)checkNV.c_str(), SQL_NTS);

    if (SQLFetch(hstmt) == SQL_SUCCESS) {
        cout << "Loi: Ma nhan vien " << maNV << " da ton tai!" << endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
    string checkTK = "SELECT MaTaiKhoan FROM TaiKhoan WHERE MaTaiKhoan = '" + maTK + "'";
    SQLExecDirect(hstmt, (SQLCHAR*)checkTK.c_str(), SQL_NTS);

    if (SQLFetch(hstmt) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
        cout << "Tai khoan chua ton tai. Tu dong tao..." << endl;
        // Băm mật khẩu mặc định 123456
        string insertAcc = "INSERT INTO TaiKhoan (MaTaiKhoan, MatKhau, PhanQuyen) "
                           "VALUES ('" + maTK + "', CONVERT(VARCHAR(64), HASHBYTES('SHA2_256', '123456'), 2), 'Employee')";
        SQLExecDirect(hstmt, (SQLCHAR*)insertAcc.c_str(), SQL_NTS);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
    string query = "INSERT INTO NhanVien (MaNhanVien, HoTen, MaTaiKhoan, Email, GioiTinh) "
                   "VALUES ('" + maNV + "', N'" + hoTen + "', '" + maTK + "', '" + email + "', N'" + gt + "')";
    if (SQL_SUCCEEDED(SQLExecDirect(hstmt, (SQLCHAR*)query.c_str(), SQL_NTS))) {
        cout << "Them nhan vien thanh cong!" << endl;
    } else {
        cout << "Them nhan vien that bai!" << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
}

void suaNhanVien(SQLHDBC dbc, string maTaiKhoanDangNhap) {
    string maNV;
    cout << "\n--- SUA THONG TIN NHAN VIEN ---" << endl;
    string phanQuyen = layPhanQuyen(dbc, maTaiKhoanDangNhap);

    if (phanQuyen == "Admin" || phanQuyen == "Manager") {
        cout << "Nhap ma nhan vien can sua: "; cin >> maNV;
    } else {
        SQLHSTMT hstmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
        string query = "SELECT MaNhanVien FROM NhanVien WHERE MaTaiKhoan = '" + maTaiKhoanDangNhap + "'";
        SQLExecDirect(hstmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQLFetch(hstmt) == SQL_SUCCESS) {
            SQLCHAR maNV_DB[50];
            SQLGetData(hstmt, 1, SQL_C_CHAR, maNV_DB, sizeof(maNV_DB), NULL);
            maNV = string((char*)maNV_DB);
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        string matKhauNhap;
        cout << "Xin chao nhan vien " << maNV << "! Ban dang dung quyen Employee." << endl;
        cout << "Vui long nhap mat khau hien tai de xac nhan: "; cin >> matKhauNhap;
        if (!kiemTraDangNhap(dbc, maTaiKhoanDangNhap, matKhauNhap)) {
            cout << "Sai mat khau! Khong co quyen chinh sua." << endl;
            return;
        }
    }
    int luaChon;
    cout << "\n1. Ho ten\n2. Mat khau\n3. Email\nChon (1-3): "; cin >> luaChon;

    SQLHSTMT hstmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
    string queryUpdate;

    if (luaChon == 1) {
        string tenMoi;
        cout << "Nhap ho ten moi: "; cin.ignore(); getline(cin, tenMoi);
        queryUpdate = "UPDATE NhanVien SET HoTen = N'" + tenMoi + "' WHERE MaNhanVien = '" + maNV + "'";
    } else if (luaChon == 2) {
        string mkMoi;
        cout << "Nhap mat khau moi: "; cin >> mkMoi;
        // Băm mật khẩu mới trước khi cập nhật
        queryUpdate = "UPDATE TaiKhoan SET MatKhau = CONVERT(VARCHAR(64), HASHBYTES('SHA2_256', '" + mkMoi + "'), 2) "
                      "WHERE MaTaiKhoan = (SELECT MaTaiKhoan FROM NhanVien WHERE MaNhanVien = '" + maNV + "')";
    } else if (luaChon == 3) {
        string emailMoi;
        cout << "Nhap email moi: "; cin >> emailMoi;
        queryUpdate = "UPDATE NhanVien SET Email = '" + emailMoi + "' WHERE MaNhanVien = '" + maNV + "'";
    } else {
        cout << "Lua chon khong hop le!" << endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return;
    }

    if (SQL_SUCCEEDED(SQLExecDirect(hstmt, (SQLCHAR*)queryUpdate.c_str(), SQL_NTS))) {
        SQLLEN rowCount;
        SQLRowCount(hstmt, &rowCount);
        if (rowCount > 0) cout << "Cap nhat thanh cong!" << endl;
        else cout << "Khong tim thay nhan vien!" << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
}

void xoaNhanVien(SQLHDBC dbc) {
    string maNV;
    cout << "\n--- XOA NHAN VIEN ---" << endl;
    cout << "Nhap ma nhan vien can xoa: "; cin >> maNV;

    SQLHSTMT hstmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);

    string querySelect = "SELECT MaTaiKhoan FROM NhanVien WHERE MaNhanVien = '" + maNV + "'";
    SQLExecDirect(hstmt, (SQLCHAR*)querySelect.c_str(), SQL_NTS);

    string maTK_CanXoa = "";
    if (SQLFetch(hstmt) == SQL_SUCCESS) {
        SQLCHAR maTK_DB[50];
        SQLGetData(hstmt, 1, SQL_C_CHAR, maTK_DB, sizeof(maTK_DB), NULL);
        maTK_CanXoa = string((char*)maTK_DB);
    } else {
        cout << "Khong tim thay nhan vien co ma: " << maNV << endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
    string queryDeleteNV = "DELETE FROM NhanVien WHERE MaNhanVien = '" + maNV + "'";
    SQLExecDirect(hstmt, (SQLCHAR*)queryDeleteNV.c_str(), SQL_NTS);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

    if (maTK_CanXoa != "") {
        SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
        string queryDeleteTK = "DELETE FROM TaiKhoan WHERE MaTaiKhoan = '" + maTK_CanXoa + "'";
        SQLExecDirect(hstmt, (SQLCHAR*)queryDeleteTK.c_str(), SQL_NTS);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    cout << "Da xoa nhan vien va tai khoan lien quan thanh cong!" << endl;
}

void timKiemNhanVien(SQLHDBC dbc) {
    string tuKhoa;
    cout << "\n--- TIM KIEM NHAN VIEN ---" << endl;
    cout << "Nhap tu khoa (Ma hoac Ten): ";
    cin.ignore(); getline(cin, tuKhoa);

    SQLHSTMT hstmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);

    string query = "SELECT n.MaNhanVien, n.HoTen, n.Email, p.TenPhongBan, n.GioiTinh "
                   "FROM NhanVien n "
                   "LEFT JOIN PhongBan p ON n.MaPhongBan = p.MaPhongBan "
                   "WHERE n.MaNhanVien LIKE '%" + tuKhoa + "%' OR n.HoTen LIKE N'%" + tuKhoa + "%'";
    SQLExecDirect(hstmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    SQLCHAR maNV[20], hoTen[100], email[100], tenPhong[100], gioiTinh[10];
    SQLLEN cbMaNV, cbHoTen, cbEmail, cbTenPhong, cbGioiTinh;
    bool coKetQua = false;

    cout << "\n--- KET QUA TIM KIEM ---" << endl;
    cout << left << setw(10) << "Ma NV" << " | "
         << setw(20) << "Ho va Ten" << " | "
         << setw(15) << "Gioi Tinh" << " | "
         << setw(25) << "Email" << " | "
         << "Phong Ban" << endl;
    cout << string(95, '-') << endl;

    while (SQLFetch(hstmt) == SQL_SUCCESS) {
        coKetQua = true;
        SQLGetData(hstmt, 1, SQL_C_CHAR, maNV, sizeof(maNV), &cbMaNV);
        SQLGetData(hstmt, 2, SQL_C_CHAR, hoTen, sizeof(hoTen), &cbHoTen);
        SQLGetData(hstmt, 3, SQL_C_CHAR, email, sizeof(email), &cbEmail);
        SQLGetData(hstmt, 4, SQL_C_CHAR, tenPhong, sizeof(tenPhong), &cbTenPhong);
        SQLGetData(hstmt, 5, SQL_C_CHAR, gioiTinh, sizeof(gioiTinh), &cbGioiTinh);

        string strMaNV((char*)maNV);
        string strHoTen((char*)hoTen);

        string strEmail = (cbEmail == SQL_NULL_DATA) ? "N/A" : string((char*)email);
        string strTenPhong = (cbTenPhong == SQL_NULL_DATA) ? "Chua phan ban" : string((char*)tenPhong);
        string strGioiTinh = (cbGioiTinh == SQL_NULL_DATA) ? "Chua ro" : string((char*)gioiTinh);

        cout << left << setw(10) << strMaNV << " | "
             << setw(20) << strHoTen << " | "
             << setw(15) << strGioiTinh << " | "
             << setw(25) << strEmail << " | "
             << strTenPhong << endl;
    }

    if (!coKetQua) {
        cout << "Khong tim thay nhan vien nao phu hop!" << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
}
