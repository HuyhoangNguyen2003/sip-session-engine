Ghi chép kiến trúc hệ thông và triển khai thực tế: 

----
 
 **Cấu trúc thư mục chuẩn:**\
    - 'include/' ('.hpp'): Khai báo giao diện class/hàm (Header files).\
    - 'src/' ('.cpp'): Cài đặt logic chi tiết (Source files).\
    - 'build/': Chứa toàn bộ file trung gian và file thực thi sau khi biên dịch (tách biệt hoàn toàn khỏi mã nguồn).\ 
    - 'docs/': Lưu trữ tài liệu thiết kế và sổ tay ôn tập.\
    - 'logs/' & 'tests/': Lưu file nhật ký chạy hệ thống và mã nguồn kiểm thử.\
 **Công cụ quản lý build ('CMakeList.txt'):**\
    - Ép chuẩn biên dịch 'C++17' ('CMAKE_CXX_STANDARD 17').\
    - Bật cờ kiểm tra lỗi nghiêm ngặt '-Wall -Wextra -Wpedantic' để trình biên dịch soi lỗi.\ 
    - Liên kết thư viện 'Threads:Threads' ('pthread' trên Linux) phục vụ lập trình đa luồng.\
 **Quản lý phiên bản ('.gitignore'):** \
    - Loại trừ thư mục 'build/' và 'log/*.log' để giữ repository sạch sẽ, chỉ lưu trữ mã nguồn thuần tuý. \

**Quy trình lưu version Git cho project:** \
   - 'git status': Kiểm tra các file vừa thêm/sửa trước khi đóng gói.\
   - 'git add': Đưa toàn bộ thay đổi vào vùng chờ (Staging).\
   - 'git commit -m "<type>: <mô tả ngắn>": Lưu mốc lịch sử tại máy Local (ví dụ: 'chore:' cho cấu hình, 'feat:' cho tính năng mới, 'fix:' cho sửa bug).\
   - 'git push': Đẩy các mốc commit mới nhất từ Ubuntu lên Github.\

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


## 1.4 Nâng cấp Header 'Logger' lên chuẩn SingleTon và Thread-safe ('include/Logger.hpp')
   - **Meyer' SingleTon Pattern:**
      - Chuyển hàm tạo 'Logger() = default;' vào 'private' để ngăn bên ngoài tự ý tạo nhiều đối tượng 'Logger'.
      - Xoá Copy Constructor và Copy Assignment ('=delete') để chặn việc nhân bản đối tượng. 
      - Cung cấp cổng truy cập duy nhất 'static Logger &getInstance()' trả về tham chiếu tới 1 thực thể cố định. 
   - **RAII (Resource Acquisition Is Initiallization):** 
      - Sử dụng hàm huỷ '~Loger()' để tự động đóng 'std::ofstream logFile_' khi chương trình kết thúc, đảm bảo không rò rỉ bộ nhớ và không mất dữ liệu đệm. 
   - **Đồng bộ đa luồng ('std::mutex')**
      - Khai báo biến khoá 'mutex_' để điều phối thứ tự ghi dữ liệu giữa các luồng, ngăn chặn xung đột bộ nhwos. 
   - **Quy ước đặt tên (Naming Convention)** Thêm hậu tố '_' cho các biến thành viên 'private' của class để dễ phân biệt với các biến cục bộ. 

## 1.5 Hoàn thiện logic SingleTon, File I/O và Mutex ('src/Logger.cpp')
   - **Magic Statics (C++11+):** Khai báo 'static Logger instance;' bên trong 'getInstance()' đảm bảo biến chỉ được khai báo 1 lần duy nhất và an toàn đa luồng mà không cần viết code khoá phức tạp.
   - **RAII với 'std::lock_guard<std::mutex>':** Tự động khoá 'mutex_' khi khởi tạo biến 'lock' và tự động mở khoá khi biến 'lock ra khỏi phạm vi hàm ('}'), loại bỏ hoàn toàn nguy cơ quên mở khoá gây treo hệ thống. 
   - **Chế độ mở file ('std::ios::out / std::ios::app):** Mở file để ghi ('out') và nối tiếp vào cuối file ('app' - append) nằm giữ lại toàn bộ chuỗi lịch sử log cũ mỗi khi khởi động lại server. 
   - **Bảo vệ khởi tạo kép (Idempotency):** Kiểm tra 'if(logFile_.is_open()) return;' trong 'init()' để tránh lỗi 'failbit' của 'std::ofstream' nếu hàm 'init()' bị gọi nhiều lần. 

## 1.6 Kiểm thử đa luồng ('std::thread') và xử lý đường dẫn File System
   **Lập trình đa luồng cơ bản ('std::thread'):**
   - Dùng 'thread.emplace_back(workerTask, i)' để khởi tạp và chạy song song nhiều luồng trực tiếp trong 'std::vector' mà không cần copy đối tượng luồng. 
   - Bắt buộc gọi 't.join()' cho mọi luồng trước khi kết thúc hàm 'main()' để luồng chính đợi các luồn con hoàn thành công việc. Lưu ý, nếu quên 'join()' thì chương trình sẽ bị lỗi 'std::terminate' và crash.
   **Quản lý đường dẫn & 'std::filesystem' (C++17):**
   - 'std::ofstream' chỉ tạo file mới khi thư mục cha đã tồn tại: nếu sai tên thư mục hoặc sai thư mục làm việc hiện hành thì hàm 'open()' sẽ thất bại. 
   - Có thể dùng 'std::filesystem::create_directories()' để tự động tạo cây thư mục cha trước khi mở file. 
   **Biên dịch với 'make':** Khi chỉ sửa nội dung bên trong các file '.cpp/.hpp' có sẵn (không thêm/xoá file mới), chỉ cần chạy 'make' trong thư mục 'build/' mà không cần chạy lại 'cmake ..'. 
---
EOF
