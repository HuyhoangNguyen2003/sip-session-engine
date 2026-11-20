# Engineering Log: Multi-threaded SIP Proxy & Session Engine

=======
> Sổ tay ghi chép kiến trúc hệ thống, tư duy C++ và các bước triển khai thực tế.

---

## Giai đoạn 0: Khởi tạo khung dự án & Quy trình Git

### 0.1 Cấu trúc thư mục chuẩn
- `include/` (`.hpp`): Khai báo giao diện class/hàm (Header files).
- `src/` (`.cpp`): Cài đặt logic chi tiết (Source files).
- `build/`: Chứa toàn bộ file trung gian và file thực thi sau khi biên dịch (tách biệt hoàn toàn khỏi mã nguồn).
- `docs/`: Lưu trữ tài liệu thiết kế và sổ tay ôn tập.
- `logs/` & `tests/`: Lưu file nhật ký chạy hệ thống và mã nguồn kiểm thử.

### 0.2 Công cụ quản lý build (`CMakeLists.txt`)
- Ép chuẩn biên dịch `C++17` (`CMAKE_CXX_STANDARD 17`).
- Bật cờ kiểm tra lỗi nghiêm ngặt `-Wall -Wextra -Wpedantic` để trình biên dịch soi lỗi sớm.
- Liên kết thư viện `Threads::Threads` (`pthread` trên Linux) phục vụ lập trình đa luồng.

### 0.3 Quản lý phiên bản (`.gitignore` & Git Workflow)
- **`.gitignore`:** Loại trừ thư mục `build/` và `logs/*.log` để giữ repository sạch sẽ, chỉ lưu trữ mã nguồn thuần túy.
- **Quy trình lưu version Git cho project:**
  - `git status`: Kiểm tra các file vừa thêm/sửa trước khi đóng gói.
  - `git add .`: Đưa toàn bộ thay đổi vào vùng chờ (Staging).
  - `git commit -m "<type>: <mô tả ngắn>"`: Lưu mốc lịch sử tại máy local (ví dụ: `chore:` cho cấu hình, `feat:` cho tính năng mới, `fix:` cho sửa bug).
  - `git push`: Đẩy các mốc commit mới nhất từ Ubuntu lên GitHub.

---

## Giai đoạn 1: Xây dựng Module Thread-Safe Logger

### 1.1 Thiết kế Header cơ bản (`include/Logger.hpp`)
- **`#pragma once`:** Ngăn trình biên dịch đọc trùng lặp một file header nhiều lần.
- **`enum class`:** Kiểu liệt kê an toàn kiểu dữ liệu (truy cập qua `LogLevel::INFO`, ...), rõ nghĩa hơn dùng hằng số nguyên.
- **Tách biệt `.hpp` và `.cpp`:** File `.hpp` chỉ chứa khai báo giao diện lớp (tên hàm, tham số); phần định nghĩa logic nằm ở `.cpp`.
- **Pass by Const Reference (`const std::string&`):** Truyền tham chiếu `&` để tránh copy dữ liệu gây tốn bộ nhớ, kết hợp `const` bảo vệ dữ liệu không bị chỉnh sửa.
- **Tính đóng gói (`public` và `private`):** Chỉ mở ra bên ngoài hàm giao tiếp chính (`log`), giấu các hàm xử lý phụ trợ (`levelToString`) vào `private`.

### 1.2 Cài đặt logic cơ bản (`src/Logger.cpp`)
- **`#include "..."` vs `#include <...>`:** Dấu `"..."` dùng cho file nội bộ của dự án, còn dấu `<...>` dùng cho thư viện chuẩn của C++.
- **`switch - case` cho `enum class`:** Tối ưu hơn `if - else` vì trình biên dịch sẽ tự động cảnh báo nhờ các cờ `-Wall -Wextra` nếu ta thêm giá trị enum mới mà không xử lý thêm điều kiện.
- **`"\n"` vs `std::endl`:** `"\n"` tốt hơn `std::endl` vì `std::endl` khiến hệ thống ép xả bộ đệm I/O (`flush`) liên tục làm giảm hiệu năng ghi log.

### 1.3 Biên dịch và chạy thử phiên bản đầu tiên của Project
- **Môi trường build trên Ubuntu:** Luôn ưu tiên cài `build-essential` và `cmake` qua `apt` thay vì `snap` để tránh giới hạn sandbox và xung đột đường dẫn hệ thống.
- **Quy trình biên dịch CMake chuẩn:**
  1. `cd` vào thư mục `build/`, chạy `cmake ..` để đọc `CMakeLists.txt` ở thư mục cha và sinh ra `Makefile`.
  2. Chạy `make` để trình biên dịch `g++` biên dịch các file `.cpp` và liên kết thành file thực thi nhị phân (`sip_engine`).
  3. Chạy `./sip_engine` thành công, xuất ra định dạng log chuẩn.

### 1.4 Nâng cấp Header `Logger` lên chuẩn Singleton và Thread-Safe (`include/Logger.hpp`)
- **Meyers' Singleton Pattern:**
  - Chuyển hàm tạo `Logger() = default;` vào `private:` để ngăn bên ngoài tự ý tạo nhiều đối tượng `Logger`.
  - Xóa Copy Constructor và Copy Assignment (`= delete`) để chặn việc nhân bản đối tượng.
  - Cung cấp cổng truy cập duy nhất `static Logger& getInstance()` trả về tham chiếu tới 1 thực thể cố định.
- **RAII (Resource Acquisition Is Initialization):**
  - Sử dụng hàm hủy `~Logger()` để tự động đóng `std::ofstream logFile_` khi chương trình kết thúc, đảm bảo không rò rỉ tài nguyên và không mất dữ liệu đệm.
- **Đồng bộ đa luồng (`std::mutex`):**
  - Khai báo biến khóa `mutex_` để điều phối thứ tự ghi dữ liệu giữa các luồng, ngăn chặn xung đột bộ nhớ (Data Race).
- **Quy ước đặt tên (Naming Convention):** Thêm hậu tố `_` cho các biến thành viên `private` của class để dễ phân biệt với các biến cục bộ.

### 1.5 Hoàn thiện logic Singleton, File I/O và Mutex (`src/Logger.cpp`)
- **Magic Statics (C++11+):** Khai báo `static Logger instance;` bên trong `getInstance()` đảm bảo biến chỉ được khởi tạo 1 lần duy nhất và an toàn đa luồng mà không cần viết code khóa phức tạp.
- **RAII với `std::lock_guard<std::mutex>`:** Tự động khóa `mutex_` khi khởi tạo biến `lock` và tự động mở khóa khi biến `lock` ra khỏi phạm vi hàm (`}`), loại bỏ hoàn toàn nguy cơ quên mở khóa gây treo hệ thống (Deadlock).
- **Chế độ mở file (`std::ios::out | std::ios::app`):** Mở file để ghi (`out`) và nối tiếp vào cuối file (`app` - append) nhằm giữ lại toàn bộ chuỗi lịch sử log cũ mỗi khi khởi động lại server.
- **Bảo vệ khởi tạo kép (Idempotency):** Kiểm tra `if (logFile_.is_open()) return;` trong `init()` để tránh lỗi `failbit` của `std::ofstream` nếu hàm `init()` bị gọi nhiều lần.

### 1.6 Kiểm thử đa luồng (`std::thread`) và xử lý đường dẫn File System
- **Lập trình đa luồng cơ bản (`std::thread`):**
  - Dùng `threads.emplace_back(workerTask, i)` để khởi tạo và chạy song song nhiều luồng trực tiếp trong `std::vector` mà không cần copy đối tượng luồng.
  - Bắt buộc gọi `t.join()` cho mọi luồng trước khi kết thúc hàm `main()` để luồng chính đợi các luồng con hoàn thành công việc. Lưu ý: nếu quên `join()`, chương trình sẽ bị lỗi `std::terminate` và crash.
- **Quản lý đường dẫn & `std::filesystem` (C++17):**
  - `std::ofstream` chỉ tạo file mới khi thư mục cha đã tồn tại; nếu sai tên thư mục hoặc sai thư mục làm việc hiện hành thì hàm `open()` sẽ thất bại.
  - Có thể dùng `std::filesystem::create_directories()` để tự động tạo cây thư mục cha trước khi mở file.
- **Biên dịch với `make`:** Khi chỉ sửa nội dung bên trong các file `.cpp`/`.hpp` có sẵn (không thêm/xóa file mới), chỉ cần chạy `make` trong thư mục `build/` mà không cần chạy lại `cmake ..`.

## Giai đoạn 2: Phân tích cấu trúc bản tin SIP (`SipMessage` & `SipParser`)

### 2.1 Thiết kế cấu trúc dữ liệu bản tin SIP (`include/SipMessage.hpp`)
- **`struct` vs `class`:** Dùng `struct` (mặc định các thành viên là `public`) cho các đối tượng thuần tuý lưu trữ và vận chuyển dữ liệu (DTO - Data Transfer Object) như `SipMessage`. 
- **`std::unordered_map<std::string, std::string>`:** Bảng băm (Hash Table) lưu trữ các cặp `Header:Value`, cho tốc độ tra cứu trung bình $O(1)$. 
-**Truy vấn an toàn với `.find()` thay vì toán tử `[]`:**
  - Toán tử `headers[key]` sẽ tự động chèn thêm phần tử mới nếu `key` chưa tồn tại (gây thay đổi dữ liệu và tốn bộ nhớ). 
  - Dùng `auto it = headers.find(key);` kết hợp kiểm tra `it !=headers.end()` giúp đọc dữ liệu an toàn (`it->second`) mà không làm thay đổi cấu trúc bảng băm. 
-**`const` Member Function (`getHeader(...) const`):** Cam kết chỉ đọc dữ liệu nội bộ, cho phép gọi hàm này trên các đối tượng `const SipMessage&`.  

### 2.2 Thiết kế Header Bộ giải mã SIP (`include/SipParser.hpp`)
- **`std::string_view` (C++17):** Chỉ là một cặp `(con trỏ + độ dài)` trỏ thẳng vào vùng nhớ chuỗi gốc mà không sap chép dữ liệu (`zero-copy`), giúp cắt chuỗi và phân tích gói tin mạng cực nhanh. 
-**`std::optional<T>`(C++17):** Biểu diễn giá trị có thể tồn tại hoặc rỗng (`std::nulopt`). Rất lý tưởng cho các hàm phân tích gói tin mạng (trả về `SipMessage` nếu hợp lệ, hoặc `std::nullopt` nếu gói tin rác/lỗi cú pháp thay vì phải dùng mã lỗi rườm rà hay ném ngoại lệ tốn kém). 
-**Hàm thành viên tĩnh (`static`):** Vì `SipParser` là một bộ xử lý không lưu trạng thái nội bộ (*stateless*), khai báo các hàm là `static` cho phép gọi trực tiếp `SipParser::parse(raw)` mà không cần khởi tạo đối tượng thừa thãi. 

### 2.3 Cài đặt logic giải mã bản tin SIP (`src/SipParser.cpp`)
- **Phân tách Header và Body:** Dựa trên chuẩn RFC3261, Phần Header và Body ngăn cách bởi một dòng trống (`\r\n\r\n` trên mạng thực tế, hoặc `\n\n` khi test trên Linux). 
- **`std::from_chars` (C++17):** Chuyển đổi chuỗi số (như `statusCode` `200`, `404`) sang `int` trực tiếp trên vùng nhớ `std::string_view` mà không cấp phát bộ nhớ động và không nén ngoại lệ (`exception`) như `std::stoi`. 

### 2.4 Kiểm thử bộ giải mã SIP (`src/main.cpp`)
- **Kiểm tra `std::optional` an toàn:** Dùng `if (parsed.has_value())` trước khi truy xuất dữ liệu bên trong qua toán tử mũi tên `parsed->...` (nếu truy cập khi rỗng sẽ gây lỗi hành vi không xác định). 
- **Cập nhật file mới với CMake:** Mỗi khi tạo thêm file `.cpp` mới trong thư mục `src/` (như `src/SipParsed.cpp`), bắt buộc phải chạy lại `cmake .. ` để cập nhật lại danh sách file mới 
---

