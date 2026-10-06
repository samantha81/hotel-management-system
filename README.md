# Hotel Management System

A console-based hotel management system written in C. It supports three user roles (Customer, Employee, Administrator) and covers room inventory, customer registration, bookings, billing with holiday discounts, staff registration, and feedback collection. All data is stored in flat files, so no database is needed.

## Features

### Customer
- Register personal details (name, email, phone, ID/passport, check-in/check-out dates) with input validation
- Browse room types with bed info, area, features, price, and number of rooms available
- Book a specific room by room type and room ID; the system calculates the stay length and total cost
- View bills for bookings, including any holiday discount
- Leave feedback

### Employee
- Register staff by job department (Cleaner, Receptionist, Security, Manager)
- View the customer list with room numbers and discounted prices
- View accommodation and see which room IDs are free for each room type
- Check a customer out (frees the room and removes the booking)
- Read customer feedback

### Administrator
- Manage room types: add, edit type name / bed info / area / features / price, or delete
- Manage individual rooms: add, update, or delete a room ID and assign it a room type
- Search customer details by name or by ID
- View the full customer list

## Requirements

- **Windows.** The program calls `system("cls")`, includes `<io.h>`, and uses the one-argument `mkdir("data")`.
- **GCC (e.g. MinGW-w64).** The code uses a GNU statement expression (`({ ... })`) and `strcasecmp` from `<strings.h>`.

## Build and Run

`accommodation.h` is included by `main.c`, so only `main.c` needs to be compiled:

```
gcc main.c -o hotel.exe
hotel.exe
```

## First-Time Setup

Before the first run, create the data folder and an empty room file. The program currently crashes if `data/room.txt` does not exist (see [Known Limitations](#known-limitations)):

```
mkdir data
type nul > data\room.txt
```

Suggested order for a fresh install:

1. Register an **Administrator** account.
2. Log in and choose **Record Accommodation Information**. First define room types, then add individual room IDs.
3. Register a **Customer** account, then fill in customer information and make a booking.
4. Register an **Employee** account to handle check-outs and feedback.

## Registration and Security Codes

Administrator and Employee registration require a security password, which is hardcoded in `registerUser()` in `main.c`:

| Role          | Security password |
|---------------|-------------------|
| Customer      | none              |
| Administrator | `AIT101`          |
| Employee      | `CST101`          |

## Menu Reference

**Main menu:** Register / Login / Exit

| Admin menu                       | Employee menu                         | Customer menu                 |
|----------------------------------|---------------------------------------|-------------------------------|
| 1. Record Accommodation Info     | 1. Register by Job Department         | 1. Complete Customer Info     |
| 2. View Customers Information    | 2. View List of Customers             | 2. View Accommodation         |
| 3. View List of Customers        | 3. View List of Accommodation         | 3. Make Booking               |
| 4. Logout                        | 4. Customer Checkout                  | 4. Give Feedback              |
|                                  | 5. View Feedback                      | 5. Logout                     |
|                                  | 6. Logout                             |                               |

### Managing room types (Admin)

Enter an existing room type name to edit it, or a new name to create it. Type `end` to leave.

When editing a type you can change its name, bed info, area, features, or price, or delete it. For features you can add several (type `end` to finish), remove one by name, or clear them all.

### Managing room IDs (Admin)

Enter a room ID. If it already exists you can update or delete it. If it doesn't, it is added and you assign it an existing room type. Type `end` to save and exit.

## Pricing and Discounts

- Total cost = room type price × number of nights (check-out minus check-in).
- A **15% discount** applies on bills and in the customer list when the check-in or check-out date falls on a public holiday or a school holiday period.
- Holiday data is hardcoded in `main.c` and covers **2025 only** (Malaysian public holidays and school holiday groups A and B).
- The stored total in `bookings.txt` is the pre-discount amount. The discount is applied when bills are displayed.

## Data Files

| File                        | Purpose                                              | Format                                       |
|-----------------------------|------------------------------------------------------|----------------------------------------------|
| `record.txt`                | User accounts                                        | Count, then `username password role` per line |
| `data/room.txt`             | Room types                                           | Raw binary dump of `Room` structs            |
| `data/accommodation.txt`    | Individual rooms                                     | Count, then `roomID availability roomType` (availability: `0` = free, `1` = booked) |
| `all_customers.txt`         | Customer details                                     | `name\|email\|phone\|id\|checkIn\|checkOut`  |
| `bookings.txt`              | Bookings                                             | `name\|roomType\|roomID\|totalCost\|checkIn\|checkOut` |
| `feedback.txt`              | Customer feedback                                    | One entry per line                           |
| `employee_records.txt`      | Registered staff                                     | `name,id,contact,department`                 |

Files are created automatically on first write, except for the `data/` folder and `data/room.txt` noted above.

## Project Structure

```
.
├── main.c             # Entry point, login/registration, role menus, customers,
│                      # bookings, billing, holidays/discounts, feedback, checkout
└── accommodation.h    # Room and Accommodation structs, file load/save,
                       # room-type and room-ID management, accommodation displays
```

Key data structures:

- `Room` (room type): type name, bed info, area, up to 20 features, price, count
- `Accommodation` (individual room): room ID, availability, room type
- `Booking`: customer name, room ID/type, days, total cost, check-in/out dates
- `User`, `Employee`, `Customer`: account, staff, and customer records

## Known Limitations

- **Missing `data/room.txt` causes a crash.** `loadRoomType()` keeps reading from the file handle after `fopen` fails, so the empty file from the setup step is required.
- **Windows-only and GCC-only** (see Requirements).
- **Passwords are stored in plain text** in `record.txt`, and the security codes are hardcoded in the source.
- **Bookings are not tied to the logged-in account.** The customer types a name when booking, and the "view booking details" screen lists all bookings, not just the current user's.
- **`data/room.txt` is a binary struct dump** despite the `.txt` extension, so it isn't human-readable or portable between compilers/platforms. Its `num` field holds the room count plus one, and the display code subtracts one.
- **Fixed size limits:** names and features are limited to 19 characters, each room type to 20 features, and the system to 100 rooms, 100 bookings, 100 users, and 100 customers.
- **Checkout bookkeeping bug:** when removing a booking, `customerCheckOut()` loops up to `userCount` instead of `bookingCount`.
- **Holiday data is static** and needs updating in `main.c` for years after 2025.
- If `data/accommodation.txt` can't be opened for writing, `saveAccommodationData()` falls back to opening `data/room.txt` by mistake.
