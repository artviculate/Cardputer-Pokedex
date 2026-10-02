This was the first run without sprite images 


#include <M5Cardputer.h>
#include <SD.h>
#include <SPI.h>

struct Pokemon {
    int id;
    String name;
    String form;
    String type1;
    String type2;
    int total;
    int hp, atk, def, spAtk, spDef, speed;
    int gen;
};

String searchQuery = "";
Pokemon currentMon;
bool found = false;
String statusInfo = "";

// Parse CSV row with flexible column counts
bool parseCsvLine(String line, Pokemon &mon) {
    line.trim();
    line.replace("\r", "");
    if (line.length() == 0) return false;

    String fields[15];
    int idx = 0;
    int start = 0;

    for (int i = 0; i < line.length(); i++) {
        if (line.charAt(i) == ',') {
            if (idx < 15) {
                fields[idx] = line.substring(start, i);
                fields[idx].trim();
                fields[idx].replace("\"", "");
            }
            idx++;
            start = i + 1;
        }
    }
    
    if (idx < 15 && start <= line.length()) {
        fields[idx] = line.substring(start);
        fields[idx].trim();
        fields[idx].replace("\"", "");
    }

    // Require at least ID (field 0) and Name (field 1)
    if (idx < 1 || fields[0].length() == 0) return false;

    mon.id    = fields[0].toInt();
    mon.name  = fields[1];
    mon.form  = (idx >= 2) ? fields[2] : "";
    mon.type1 = (idx >= 3) ? fields[3] : "";
    mon.type2 = (idx >= 4) ? fields[4] : "";
    mon.total = (idx >= 5) ? fields[5].toInt() : 0;
    mon.hp    = (idx >= 6) ? fields[6].toInt() : 0;
    mon.atk   = (idx >= 7) ? fields[7].toInt() : 0;
    mon.def   = (idx >= 8) ? fields[8].toInt() : 0;
    mon.spAtk = (idx >= 9) ? fields[9].toInt() : 0;
    mon.spDef = (idx >= 10) ? fields[10].toInt() : 0;
    mon.speed = (idx >= 11) ? fields[11].toInt() : 0;
    mon.gen   = (idx >= 12) ? fields[12].toInt() : 1;

    return true;
}

// Search SD card file for matching Name or Dex ID
bool searchPokedex(String query) {
    statusInfo = "";
    
    File file = SD.open("/pokemon.csv", FILE_READ);
    if (!file) file = SD.open("/pokemon", FILE_READ);
    if (!file) file = SD.open("/pokemon.csv.csv", FILE_READ);
    if (!file) file = SD.open("/POKEMON.CSV", FILE_READ);

    if (!file) {
        statusInfo = "Error: File not found on SD!";
        return false;
    }

    query.toLowerCase();
    query.trim();

    int linesRead = 0;
    int parsedRows = 0;

    while (file.available()) {
        String line = file.readStringUntil('\n');
        linesRead++;
        line.trim();
        line.replace("\r", "");

        if (line.length() == 0) continue;
        if (line.startsWith("ID,") || line.startsWith("id,") || line.startsWith("\"ID\"")) continue;

        Pokemon tempMon;
        if (parseCsvLine(line, tempMon)) {
            parsedRows++;
            String checkName = tempMon.name;
            checkName.toLowerCase();

            // Match exact Dex ID or partial/exact Name
            if (query == String(tempMon.id) || checkName.equalsIgnoreCase(query) || checkName.startsWith(query)) {
                currentMon = tempMon;
                file.close();
                return true;
            }
        }
    }
    file.close();

    if (linesRead == 0) {
        statusInfo = "Error: CSV file is empty!";
    } else if (parsedRows == 0) {
        statusInfo = "Error: Couldn't parse CSV rows!";
    } else {
        statusInfo = "No match (" + String(parsedRows) + " rows read)";
    }

    return false;
}

// Render the UI on the Cardputer display
void drawUI() {
    M5Cardputer.Display.fillScreen(BLACK);
    
    // Top Bar - Search Input
    M5Cardputer.Display.fillRect(0, 0, 240, 20, BLUE);
    M5Cardputer.Display.setTextColor(WHITE);
    M5Cardputer.Display.setTextSize(1.5);
    M5Cardputer.Display.setCursor(5, 3);
    M5Cardputer.Display.printf("Find: %s_", searchQuery.c_str());

    if (found) {
        M5Cardputer.Display.setTextColor(YELLOW);
        M5Cardputer.Display.setCursor(5, 25);
        if (currentMon.form.length() > 0) {
            M5Cardputer.Display.printf("#%04d %s (%s)", currentMon.id, currentMon.name.c_str(), currentMon.form.c_str());
        } else {
            M5Cardputer.Display.printf("#%04d %s [G%d]", currentMon.id, currentMon.name.c_str(), currentMon.gen);
        }

        M5Cardputer.Display.setTextColor(CYAN);
        M5Cardputer.Display.setCursor(5, 42);
        if (currentMon.type2.length() > 0) {
            M5Cardputer.Display.printf("Type: %s/%s | BST: %d", currentMon.type1.c_str(), currentMon.type2.c_str(), currentMon.total);
        } else {
            M5Cardputer.Display.printf("Type: %s | BST: %d", currentMon.type1.c_str(), currentMon.total);
        }

        M5Cardputer.Display.setTextColor(WHITE);
        M5Cardputer.Display.setCursor(5, 62);
        M5Cardputer.Display.printf("HP : %-3d  ATK: %-3d  DEF: %-3d", currentMon.hp, currentMon.atk, currentMon.def);
        M5Cardputer.Display.setCursor(5, 78);
        M5Cardputer.Display.printf("SpA: %-3d  SpD: %-3d  SPD: %-3d", currentMon.spAtk, currentMon.spDef, currentMon.speed);
    } else if (statusInfo.length() > 0) {
        M5Cardputer.Display.setTextColor(RED);
        M5Cardputer.Display.setCursor(5, 40);
        M5Cardputer.Display.println(statusInfo);
    } else {
        M5Cardputer.Display.setTextColor(DARKGREY);
        M5Cardputer.Display.setCursor(5, 40);
        M5Cardputer.Display.println("Type a name or # & hit ENTER");
    }
}

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(BLACK);

    SPI.begin(40, 39, 14, 12);
    if (!SD.begin(12, SPI, 20000000)) {
        M5Cardputer.Display.setTextColor(RED);
        M5Cardputer.Display.setCursor(5, 20);
        M5Cardputer.Display.println("SD Init Failed!");
        while (1) delay(100);
    }

    drawUI();
}

void loop() {
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            auto status = M5Cardputer.Keyboard.keysState();

            for (auto c : status.word) {
                if (c >= 32 && c <= 126 && searchQuery.length() < 15) {
                    searchQuery += c;
                }
            }

            if (status.del && searchQuery.length() > 0) {
                searchQuery.remove(searchQuery.length() - 1);
            }

            if (status.enter && searchQuery.length() > 0) {
                found = searchPokedex(searchQuery);
            }

            drawUI();
        }
    }
}
