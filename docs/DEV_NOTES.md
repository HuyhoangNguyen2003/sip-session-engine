Ghi chép kiến trúc hệ thông và triển khai thực tế: 

----
 
 ** Cấu trúc thư mục chuẩn: 
    - 'include/' ('.hpp'): Khai báo giao diện class/hàm (Header files). 
    - 'src/' ('.cpp'): Cài đặt logic chi tiết (Source files). 
    - 'build/': Chứa toàn bộ file trung gian và file thực thi sau khi biên dịch (tách biệt hoàn toàn khỏi mã nguồn). 
    - 'docs/': Lưu trữ tài liệu thiết kế và sổ tay ôn tập. 
    - 'logs/' & 'tests/': Lưu file nhật ký chạy hệ thống và mã nguồn kiểm thử. 
 ** Công cụ quản lý build ('CMakeList.txt'): 
    - Ép chuẩn biên dịch 'C++17' ('CMAKE_CXX_STANDARD 17').
    - Bật cờ kiểm tra lỗi nghiêm ngặt '-Wall -Wextra -Wpedantic' để trình biên dịch soi lỗi. 
    - Liên kết thư viện 'Threads:Threads' ('pthread' trên Linux) phục vụ lập trình đa luồng. 
 ** Quản lý phiên bản ('.gitignore'): 
    - Loại trừ thư mục 'build/' và 'log/*.log' để giữ repository sạch sẽ, chỉ lưu trữ mã nguồn thuần tuý. 

---
EOF