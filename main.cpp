#include <iostream>
#include <fstream>
#include <vector>
#include "icalendarlib/icalendar.h"

using namespace std;

struct EventInfo {
    std::string Summary;
    Date DtStart;
    Date DtEnd;
    std::string Location;
    std::string Description;
};

void addEvent(ICalendar& cal,
              const string& summary,
              const Date& startTime = Date(),
              const Date& endTime = Date(),
              const string& location = "",
              const string& description = "") {
    Event *newEvent = new Event;
    newEvent->Summary = summary;
    newEvent->DtStart = startTime.IsEmpty() ? (Date().SetToNow(), startTime) : startTime;
    if (endTime.IsEmpty()) {
        newEvent->DtEnd = newEvent->DtStart;
        newEvent->DtEnd[MINUTE] += 30;
    } else {
        newEvent->DtEnd = endTime;
    }
    newEvent->Categories = location;
    newEvent->Description = description;
    cal.AddEvent(newEvent);
}

void deleteEvent(ICalendar& cal, Event* oldEvent) {
    cal.DeleteEvent(oldEvent);
}

Event* findEventByUID(ICalendar& cal, const string& uid) {
    ICalendar::Query query(&cal);
    Event* e = nullptr;
    while ((e = query.GetNextEvent(false)) != nullptr) {
        if (e->UID == uid) return e;
    }
    return nullptr;
}

bool modifyEvent(ICalendar& cal, Event* oldEvent,
                 const string& newSummary,
                 const Date& newStart,
                 const Date& newEnd,
                 const string& newLocation = "",
                 const string& newDescription = "") {
    oldEvent->Summary = newSummary;
    oldEvent->DtStart = newStart;
    oldEvent->DtEnd = newEnd;
    oldEvent->Categories = newLocation;
    oldEvent->Description = newDescription;
    cal.ModifyEvent(oldEvent);
    return true;
}

vector<EventInfo> getEventsInRange(ICalendar& cal, const Date& start, const Date& end) {
    vector<EventInfo> eventsInRange;
    ICalendar::Query query(&cal);
    query.Criteria.From = start;
    query.Criteria.To = end;
    query.ResetPosition();
    Event* currentEvent = nullptr;

    while ((currentEvent = query.GetNextEvent(false)) != nullptr) {
        EventInfo info;
        info.Summary = currentEvent->Summary;
        info.DtStart = currentEvent->DtStart;
        info.DtEnd = currentEvent->DtEnd;
        info.Location = currentEvent->Categories; // location
        info.Description = currentEvent->Description;
        eventsInRange.push_back(info);
    }

    return eventsInRange;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <calendar_file> <output_file> <command> [args...]\n";
        return 1;
    }

    string calendarFile = argv[1];
    string outputFile = argv[2];
    string command = argv[3];

    ICalendar cal(calendarFile.data());

    if (command == "add") {
        if (argc < 6) {
            cerr << "Usage: add <summary> <start> <end> [location] [description]\n";
            return 1;
        }
        string summary = argv[4];
        Date start = argv[5] ? Date(string(argv[5])) : Date();
        Date end = (argc > 6 && argv[6]) ? Date(string(argv[6])) : Date();
        string location = (argc > 7) ? argv[7] : "";
        string description = (argc > 8) ? argv[8] : "";

        addEvent(cal, summary, start, end, location, description);
        cout << "Event added: " << summary << endl;
    }
    else if (command == "modify") {
        if (argc < 8) {
            cerr << "Usage: modify <uid> <newSummary> <newStart> <newEnd> [newLocation] [newDescription]\n";
            return 1;
        }
        string uid = argv[4];
        Event* e = findEventByUID(cal, uid);
        if (!e) { cerr << "Event UID not found: " << uid << endl; return 1; }

        string newSummary = argv[5];
        Date newStart = Date(argv[6]);
        Date newEnd = Date(argv[7]);
        string newLocation = (argc > 8) ? argv[8] : "";
        string newDescription = (argc > 9) ? argv[9] : "";

        modifyEvent(cal, e, newSummary, newStart, newEnd, newLocation, newDescription);
        cout << "Event modified: " << newSummary << endl;
    }
    else if (command == "delete") {
        if (argc < 5) { cerr << "Usage: delete <uid>\n"; return 1; }
        string uid = argv[4];
        Event* e = findEventByUID(cal, uid);
        if (!e) { cerr << "Event UID not found: " << uid << endl; return 1; }
        deleteEvent(cal, e);
        cout << "Event deleted: " << uid << endl;
    }
    else if (command == "list") {
        if (argc < 6) { cerr << "Usage: list <start> <end>\n"; return 1; }
        Date start(argv[4]);
        Date end(argv[5]);
        vector<EventInfo> events = getEventsInRange(cal, start, end);

        ofstream out(outputFile);
        out << "[\n";
        for (size_t i = 0; i < events.size(); ++i) {
            out << "  {\n";
            out << "    \"Summary\": \"" << events[i].Summary << "\",\n";
            out << "    \"StartDate\": \"" << events[i].DtStart.Format() << "\",\n";
            out << "    \"EndDate\": \"" << events[i].DtEnd.Format() << "\",\n";
            out << "    \"Location\": \"" << events[i].Location << "\",\n";
            out << "    \"Description\": \"" << events[i].Description << "\"\n";
            out << "  }" << (i + 1 < events.size() ? "," : "") << "\n";
        }
        out << "]";
        out.close();
        cout << "Events written to " << outputFile << endl;
    }
    else {
        cerr << "Unknown command: " << command << endl;
        return 1;
    }

    return 0;
}
