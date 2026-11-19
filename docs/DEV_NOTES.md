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

** Quy trình lưu version Git cho project: 
   - 'git status': Kiểm tra các file vừa thêm/sửa trước khi đóng gói. 
   - 'git add': Đưa toàn bộ thay đổi vào vùng chờ (Staging). 
   - 'git commit -m "<type>: <mô tả ngắn>": Lưu mốc lịch sử tại máy Local (ví dụ: 'chore:' cho cấu hình, 'feat:' cho tính năng mới, 'fix:' cho sửa bug). 
   - 'git push': Đẩy các mốc commit mới nhất từ Ubuntu lên Github.  

## 1.1 Thiết kế Header cơ bản ('include/Logger.hpp')
   -'#pragma once': Ngăn trình biên dịch đọc trùng lặp một file header nhiều lần.
   -'enum class': Kiểu liệt kê an toàn kiểu dữ liệu (Truy cập qua 'LogLevel::INFO',...), rõ nghĩa hơn dùng hằng số nguyên. 
   - Tách biệt '.hpp' và '.cpp': File '.hpp' chỉ chứa khai báo giao diện lớp (tên hàm, tham số); phần định nghĩa logic nằm ở '.cpp'. 
   - pass by const reference ('const std::string&'): Truyền tham chiếu "&" để tránh copy dữ liệu gây tốn bộ nhớ, kết hợp 'const' bảo vệ dữ liệu ko bị chỉnh sửa. 
   - Tính đóng gói ('public' và 'private'): Chỉ mở ra bên ngoài hàm giao tiếp chính ('log'), giấu các hàm xử lý phụ trợ ('levelToString') vào 'private'. 


## 1.2 Cài đặt logic cơ bản ('src/Logger.cpp)
   - #include "..." vs #include <...>: Dấu "..." dùng cho file nội bộ của dự án, còn dấu <...> dùng cho thư việc chuẩn của C++ 
   - switch - case cho 'enum class': Tối ưu hơn 'if-else' vì trình biên dịch sẽ tự động cảnh báo nhờ các cờ '-Wall -Wextra' nếu ta thêm giá trị enum mới mà không xử lý thêm điều kiện. 
   - '\n' tốt hơn 'endl' vì endl khiến hệ thống ép xả bộ đệm I/O liên tục làm giảm hiệu năng ghi log.


## 1.3 Biên dịch và chạy thử phiên bản đầU tiên của Project
   - Môi trường build trên Ubuntu: Luôn ưu tiên cài 'buidl-essential' và 'cmake' qua 'apt' thay vì 'snap' để tránh giới hạn sandbox và xung đột đường dẫn hệ thống. 
   - **Quy trình biên dịch Cmake chuẩn**
   1/ cd vào thư mục build, chạy 'cmake ..' để đọc 'CmakeLists.txt' ở thư mục cha và sinh ra 'Makefile'.
   2/ Chạy 'make' để trình biên dịch 'g++' biên dịch các file '.cpp' và liên kết thành file thực thi nhị phân (sip_engine).
   **Kết quả** 
   3/ Chạy './sip_engine' thành công, xuấT ra định dạng log chuẩn.

   
---
EOF