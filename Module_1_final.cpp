#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
using namespace std;

struct record {
    int flightID = 0;
    int departure = 0;
    int arrival = 0;
    int estimatedArrival = 0;
    int duration = 0;
    int price = 0;

    string source, destination, airline, status;

    record* right = nullptr;
    record* left = nullptr;
};

class FlightManagementSystem {
private:
    record* root = nullptr;
    const string filename = "flights.txt";

public:
    int noflights = 0;

    bool equalsIgnoreCase(const string& a, const string& b) {
        if (a.size() != b.size())
            return false;

        for (size_t i = 0; i < a.size(); ++i) {
            if (tolower(a[i]) != tolower(b[i]))
                return false;
        }

        return true;
    }

    bool inputTime(string label, int& time) {
        int hour, minute;

        cout << "Enter " << label << " hour (0-23): ";
        cin >> hour;

        if (hour < 0 || hour > 23) {
            cout << "Invalid hour. Enter a value between 0 and 23.\n";
            return false;
        }

        cout << "Enter " << label << " minutes (0-59): ";
        cin >> minute;

        if (minute < 0 || minute > 59) {
            cout << "Invalid minutes. Enter a value between 0 and 59.\n";
            return false;
        }

        time = hour * 60 + minute;

        return true;
    }

    string formatTime(int time) {
        time = time % 1440;

        int hour = time / 60;
        int minute = time % 60;

        string h = (hour < 10 ? "0" : "") + to_string(hour);
        string m = (minute < 10 ? "0" : "") + to_string(minute);

        return h + ":" + m;
    }

    int calculateArrival(int departure, int duration) {
        return departure + duration;
    }

    void updateStatus(record* flight) {
        if (equalsIgnoreCase(flight->status, "Cancelled"))
            return;

        int scheduledArrival = flight->arrival;

        if (flight->arrival < flight->departure)
            scheduledArrival += 1440;

        flight->estimatedArrival =
            calculateArrival(flight->departure, flight->duration);

        if (flight->estimatedArrival > scheduledArrival)
            flight->status = "Delayed";
        else
            flight->status = "Scheduled";
    }

    bool insert(record* node) {
        if (root == nullptr) {
            root = node;
            noflights++;
            return true;
        }

        record* y = root;

        while (true) {
            if (node->flightID < y->flightID) {
                if (y->left == nullptr) {
                    y->left = node;
                    break;
                }

                y = y->left;
            }
            else if (node->flightID > y->flightID) {
                if (y->right == nullptr) {
                    y->right = node;
                    break;
                }

                y = y->right;
            }
            else {
                return false;
            }
        }

        noflights++;
        return true;
    }

    void savePreorder(record* node, ofstream& out) {
        if (node == nullptr)
            return;

        out << node->flightID << '|'
            << node->departure << '|'
            << node->arrival << '|'
            << node->estimatedArrival << '|'
            << node->duration << '|'
            << node->price << '|'
            << node->source << '|'
            << node->destination << '|'
            << node->airline << '|'
            << node->status << '\n';

        savePreorder(node->left, out);
        savePreorder(node->right, out);
    }

    void printRecord(record* node) {
        cout << "Flight ID: " << node->flightID << endl;
        cout << "Flight Departure Time: "
            << formatTime(node->departure) << endl;
        cout << "Scheduled Arrival Time: "
            << formatTime(node->arrival) << endl;
        cout << "Estimated Arrival Time: "
            << formatTime(node->estimatedArrival) << endl;
        cout << "Flight Duration: " << node->duration << " minutes" << endl;
        cout << "Price: " << node->price << endl;
        cout << "Flight Source: " << node->source << endl;
        cout << "Flight Destination: " << node->destination << endl;
        cout << "Flight Airline: " << node->airline << endl;
        cout << "Status: " << node->status << endl;
    }

    void inorder(record* node) {
        if (node == nullptr)
            return;

        inorder(node->left);
        printRecord(node);
        cout << endl;
        inorder(node->right);
    }

    void destroy(record* node) {
        if (node == nullptr)
            return;

        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    void collectByRoute(record* node, const string& src,
        const string& dst, record**& arr,
        int& count, int& capacity) {

        if (node == nullptr)
            return;

        collectByRoute(node->left, src, dst, arr, count, capacity);

        if (equalsIgnoreCase(node->source, src) &&
            equalsIgnoreCase(node->destination, dst) &&
            !equalsIgnoreCase(node->status, "Cancelled")) {

            if (count == capacity) {
                int newCap = (capacity == 0) ? 4 : capacity * 2;

                record** bigger = new record * [newCap];

                for (int i = 0; i < count; ++i)
                    bigger[i] = arr[i];

                delete[] arr;
                arr = bigger;
                capacity = newCap;
            }

            arr[count++] = node;
        }

        collectByRoute(node->right, src, dst, arr, count, capacity);
    }

    void sortByPrice(record** arr, int n) {
        for (int i = 0; i < n - 1; ++i) {
            for (int j = 0; j < n - 1 - i; ++j) {

                if (arr[j]->price > arr[j + 1]->price) {
                    record* temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }

    void printCompact(record* node) {
        cout << "  [ID " << node->flightID << "] "
            << node->source << " -> " << node->destination
            << " | Dep: " << formatTime(node->departure)
            << " Arr: " << formatTime(node->arrival)
            << " ETA: " << formatTime(node->estimatedArrival)
            << " | " << node->duration << " min"
            << " | $" << node->price
            << " | " << node->airline
            << " (" << node->status << ")\n";
    }

    record* findByID(int id) {
        record* y = root;

        while (y != nullptr && y->flightID != id) {
            if (id < y->flightID)
                y = y->left;
            else
                y = y->right;
        }

        return y;
    }

    void addflight() {
        record* temp = new record();

        cout << "Enter Flight ID: ";
        cin >> temp->flightID;

        if (!inputTime("departure", temp->departure)) {
            delete temp;
            return;
        }

        if (!inputTime("scheduled arrival", temp->arrival)) {
            delete temp;
            return;
        }

        cout << "Enter flight duration in minutes: ";
        cin >> temp->duration;

        cout << "Enter flight price: ";
        cin >> temp->price;

        if (temp->duration <= 0 || temp->price < 0) {
            cout << "Duration must be positive and price cannot be negative.\n";
            delete temp;
            return;
        }

        cin.ignore(10000, '\n');

        cout << "Enter source: ";
        getline(cin, temp->source);

        cout << "Enter destination: ";
        getline(cin, temp->destination);

        cout << "Enter airline: ";
        getline(cin, temp->airline);

        temp->status = "Scheduled";

        updateStatus(temp);

        cout << "\nCalculated estimated arrival time: "
            << formatTime(temp->estimatedArrival) << endl;

        if (!insert(temp)) {
            cout << "Duplicate flight ID - not inserted.\n";
            delete temp;
            return;
        }

        cout << "Flight added successfully." << endl;
        cout << "Automatically assigned status: "
            << temp->status << endl;
    }

    void search(int key) {
        record* y = findByID(key);

        if (y == nullptr) {
            cout << "Flight not in system" << endl;
            return;
        }

        printRecord(y);
    }

    void deleteflight(int key) {
        record* y = root;
        record* parent = nullptr;

        while (y != nullptr && y->flightID != key) {
            parent = y;

            if (key < y->flightID)
                y = y->left;
            else
                y = y->right;
        }

        if (y == nullptr) {
            cout << "Flight not found" << endl;
            return;
        }

        if (y->left != nullptr && y->right != nullptr) {

            record* succParent = y;
            record* succ = y->right;

            while (succ->left != nullptr) {
                succParent = succ;
                succ = succ->left;
            }

            y->flightID = succ->flightID;
            y->departure = succ->departure;
            y->arrival = succ->arrival;
            y->estimatedArrival = succ->estimatedArrival;
            y->duration = succ->duration;
            y->price = succ->price;
            y->source = succ->source;
            y->destination = succ->destination;
            y->airline = succ->airline;
            y->status = succ->status;

            parent = succParent;
            y = succ;
        }

        record* child = (y->left != nullptr) ? y->left : y->right;

        if (parent == nullptr)
            root = child;
        else if (parent->left == y)
            parent->left = child;
        else
            parent->right = child;

        delete y;
        noflights--;

        cout << "Flight deleted." << endl;
    }

    void displayall() {
        if (root == nullptr) {
            cout << "No flights in the system." << endl;
            return;
        }

        inorder(root);
    }

    void saveToFile() {
        ofstream out(filename);

        if (!out) {
            cout << "Could not open " << filename << " for writing." << endl;
            return;
        }

        savePreorder(root, out);

        if (!out) {
            cout << "Error while saving flights.\n";
            return;
        }

        cout << "Saved " << noflights
            << " flight(s) to " << filename << "." << endl;
    }

    void loadFromFile() {
        ifstream in(filename);

        if (!in) {
            cout << "Could not open " << filename << " for reading." << endl;
            return;
        }

        destroy(root);
        root = nullptr;
        noflights = 0;

        string line;

        while (getline(in, line)) {
            if (line.empty())
                continue;

            stringstream ss(line);
            string field;

            record* r = new record();

            getline(ss, field, '|');
            r->flightID = stoi(field);

            getline(ss, field, '|');
            r->departure = stoi(field);

            getline(ss, field, '|');
            r->arrival = stoi(field);

            getline(ss, field, '|');
            r->estimatedArrival = stoi(field);

            getline(ss, field, '|');
            r->duration = stoi(field);

            getline(ss, field, '|');
            r->price = stoi(field);

            getline(ss, r->source, '|');
            getline(ss, r->destination, '|');
            getline(ss, r->airline, '|');
            getline(ss, r->status);

            updateStatus(r);

            if (!insert(r))
                delete r;
        }

        cout << "Loaded " << noflights
            << " flight(s) from " << filename << "." << endl;
    }

    void roundTripSearch() {
        if (root == nullptr) {
            cout << "No flights in system.\n";
            return;
        }

        cin.ignore(10000, '\n');

        string src, dst;

        cout << "Enter origin city: ";
        getline(cin, src);

        cout << "Enter destination city: ";
        getline(cin, dst);

        if (equalsIgnoreCase(src, dst)) {
            cout << "Origin and destination must be different.\n";
            return;
        }

        record** outbound = nullptr;
        int outCount = 0, outCap = 0;

        record** returning = nullptr;
        int retCount = 0, retCap = 0;

        collectByRoute(root, src, dst, outbound, outCount, outCap);
        collectByRoute(root, dst, src, returning, retCount, retCap);

        if (outCount == 0) {
            cout << "\nNo outbound flights from "
                << src << " to " << dst << ".\n";
        }
        else if (retCount == 0) {
            cout << "\nNo return flights from "
                << dst << " to " << src << ".\n";
        }
        else {
            sortByPrice(outbound, outCount);
            sortByPrice(returning, retCount);

            cout << "\n OUTBOUND: "
                << src << " -> " << dst << " \n";

            for (int i = 0; i < outCount; ++i)
                printCompact(outbound[i]);

            cout << "\n RETURN: "
                << dst << " -> " << src << " \n";

            for (int i = 0; i < retCount; ++i)
                printCompact(returning[i]);

            cout << "\n CHEAPEST ROUND TRIP \n";

            cout << "Outbound: ";
            printCompact(outbound[0]);

            cout << "Return: ";
            printCompact(returning[0]);

            cout << "TOTAL PRICE: "
                << (outbound[0]->price + returning[0]->price) << "\n";
        }

        delete[] outbound;
        delete[] returning;
    }

    ~FlightManagementSystem() {
        destroy(root);
    }
};

int main() {
    FlightManagementSystem hi;

    int choice = 0, key = 0;

    do {
        cout << "\n===== Flight Management System =====" << endl;
        cout << "1. Add a flight" << endl;
        cout << "2. Search for a flight" << endl;
        cout << "3. Delete a flight" << endl;
        cout << "4. Display all flights" << endl;
        cout << "5. Search round-trip flights" << endl;
        cout << "6. Save flights to file" << endl;
        cout << "7. Load flights from file" << endl;
        cout << "8. Exit" << endl;
        cout << "Choice: ";

        cin >> choice;

        switch (choice) {

        case 1:
            hi.addflight();
            break;

        case 2:
            if (hi.noflights == 0)
                cout << "There are no flights." << endl;
            else {
                cout << "Enter the flight ID that needs to be searched: ";

                cin >> key;
                hi.search(key);
            }
            break;

        case 3:
            if (hi.noflights == 0)
                cout << "There are no flights." << endl;
            else {
                cout << "Enter the flight ID that needs to be deleted: ";

                cin >> key;
                hi.deleteflight(key);
            }
            break;

        case 4:
            hi.displayall();
            break;

        case 5:
            hi.roundTripSearch();
            break;

        case 6:
            hi.saveToFile();
            break;

        case 7:
            hi.loadFromFile();
            break;

        case 8:
            cout << "Goodbye!" << endl;
            break;

        default:
            cout << "Invalid choice!!!" << endl;
        }

    } while (choice != 8);

    return 0;
}