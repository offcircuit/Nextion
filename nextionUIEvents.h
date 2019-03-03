#ifndef NEXTION_EVENTS_H
#define NEXTION_EVENTS_H

class NextionEvents {
  private:
    typedef void (*Function) ();
    typedef struct Event {
      int8_t page, id;
      bool type;
    };

  public:
    typedef struct Callback {
      Function pointer = NULL;
      Event event = {};
    };

    Callback callbacks[NEXTION_EVENT_LIMIT];
    uint8_t count = 0;

    int8_t NextionEvents::add(Event event, Function pointer) {
      int8_t id = find(event);
      if (id > -1) {
        callbacks[id].pointer = pointer;
        return id;
      }
      callbacks[count].event.page = event.page;
      callbacks[count].event.id = event.id;
      callbacks[count].event.type = event.type;
      callbacks[count++].pointer = pointer;
      return count;
    }

    bool NextionEvents::remove(Event event) {
      int8_t id = find(event);
      if (id > -1) {
        count--;
        for (uint8_t i = id; i < count; i++) callbacks[i] = callbacks[i + 1];
        return true;
      }
      return false;
    }

    int8_t NextionEvents::find(Event event) {
      for (uint8_t i = 0; i < count; i++)
        if ((callbacks[i].event.page == event.page) && (callbacks[i].event.id == event.id) && ((bool)callbacks[i].event.type == (bool)event.type)) return i;
      return -1;
    }

    void NextionEvents::list() {
      for (uint8_t i = 0; i < count; i++) {
        Serial.print("item [");  Serial.print(callbacks[i].event.page); Serial.print(", ");
        Serial.print(callbacks[i].event.id); Serial.print(", "); Serial.print(callbacks[i].event.type); Serial.println("] ...");
      }
    }
};

#endif
