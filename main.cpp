#include <iostream>
#include <windows.h>
#include <sqlext.h>

using namespace std;

int main() {
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret;

    // 1. Khởi tạo môi trường
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);

    // 2. Chuỗi kết nối (Connection String) - Nơi giao tiếp với Database
    SQLCHAR outstr[1024];
    SQLSMALLINT outstrlen;
    ret = SQLDriverConnect(dbc, NULL,
        (SQLCHAR*)"DRIVER={ODBC Driver 17 for SQL Server};SERVER=localhost;DATABASE=QuanLyNhanSu;Trusted_Connection=yes;",
        SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_NOPROMPT);

    // 3. Kiểm tra kết quả và Thêm dữ liệu
    if (SQL_SUCCEEDED(ret)) {
        cout << "Ket noi Database thanh cong!" << endl;

        // Tạo một "phiên dịch viên" (Statement Handle) để mang lệnh SQL đi
        SQLHSTMT stmt;
        SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

        // Chuẩn bị câu lệnh SQL: Thêm 1 Tài khoản và 1 Nhân viên tương ứng
        SQLCHAR* sqlQuery = (SQLCHAR*)
            "INSERT INTO TaiKhoan (MaTaiKhoan, MatKhau, PhanQuyen) VALUES ('NV001', 'chuoi_bam_cua_123456', 'Manager'); "
            "INSERT INTO NhanVien (MaNhanVien, HoTen, MaTaiKhoan) VALUES ('NV001', N'Nguyen Van Sep', 'NV001');";

        // Yêu cầu C++ thực thi câu lệnh SQL trên
        SQLRETURN retCode = SQLExecDirect(stmt, sqlQuery, SQL_NTS);

        if (SQL_SUCCEEDED(retCode)) {
            cout << "Bom du lieu mau thanh cong!" << endl;
        } else {
            cout << "Bom du lieu that bai!" << endl;
        }

        // Dọn dẹp sau khi dùng xong
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    } else {
        cout << "Ket noi that bai. Vui long kiem tra lai chuoi ket noi." << endl;
    }

    // 4. Dọn dẹp
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);

    return 0;
}
