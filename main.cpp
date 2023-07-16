#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define T 8
#define STRING_SIZE 256
#define SECTION_SIZE 1280

// Struktury danych
typedef struct Attribute {
    char* name;
    char* value;
    struct Attribute* next;
} Attribute;

typedef struct Selector {
    char* name;
    struct Selector* next;
} Selector;

typedef struct Section {
    int countSelectors;
    int countAttributes;
    Selector* selectors;
    Attribute* attributes;
} Section;

typedef struct Node {
    Section* sections[T];
    int count;
    int deleted;
    struct Node* next;
    struct Node* prev;
} Node;

// stan wczytywania
enum { DATA, COMMAND };

Attribute* createAttribute(char* name, char* value) {
    Attribute* attribute = (Attribute*)malloc(sizeof(Attribute));
    attribute->name = (char*)malloc(strlen(name) + 1);
    attribute->value = (char*)malloc(strlen(value) + 1);
    attribute->next = NULL;
    strcpy(attribute->name, name);
    strcpy(attribute->value, value);
    return attribute;
}

void addAttribute(Section* section, Attribute* attribute) {
    if (section->attributes == NULL) {
        section->attributes = attribute;
    }
    else {
        Attribute* current = section->attributes;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = attribute;
    }
    section->countAttributes++;
}

void deleteAttribute(Section* section, Attribute* attribute) {
    Attribute* prevAttribute = NULL;
    Attribute* currentAttribute = section->attributes;

    while (currentAttribute != NULL) {
        if (currentAttribute == attribute) {
            if (prevAttribute == NULL) {
                section->attributes = currentAttribute->next;
            }
            else {
                prevAttribute->next = currentAttribute->next;
            }
            free(currentAttribute->name);
            free(currentAttribute->value);
            free(currentAttribute);
            section->countAttributes--;
            break;
        }
        prevAttribute = currentAttribute;
        currentAttribute = currentAttribute->next;
    }
}

Selector* createSelector(char* name) {
    Selector* selector = (Selector*)malloc(sizeof(Selector));
    selector->name = (char*)malloc(strlen(name) + 1);
    selector->next = NULL;
    strcpy(selector->name, name);
    return selector;
}

void addSelector(Section* section, Selector* selector) {
    if (section->selectors == NULL) {
        section->selectors = selector;
    }
    else {
        Selector* current = section->selectors;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = selector;
    }
    section->countSelectors++;
}

void deleteSelector(Section* section, Selector* selector) {
    Selector* prevSelector = NULL;
    Selector* currentSelector = section->selectors;

    while (currentSelector != NULL) {
        if (currentSelector == selector) {
            if (prevSelector == NULL) {
                section->selectors = currentSelector->next;
            }
            else {
                prevSelector->next = currentSelector->next;
            }
            free(currentSelector->name);
            free(currentSelector);
            section->countSelectors--;
            break;
        }
        prevSelector = currentSelector;
        currentSelector = currentSelector->next;
    }
}

Section* createSection() {
    Section* section = (Section*)malloc(sizeof(Section));
    section->countAttributes = 0;
    section->countSelectors = 0;
    section->selectors = NULL;
    section->attributes = NULL;
    return section;
}


Node* createNode() {
    Node* node = (Node*)malloc(sizeof(Node));
    node->count = 0;
    node->deleted = 0;
    for (int i = 0; i < T; i++) {
        node->sections[i] = NULL;
    }
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void addSection(Node** node, Section* section) {
    Node* curr_node = *node;
    while (curr_node->count == T) {
        Node* tmp = curr_node;
        curr_node = curr_node->next;
        if (curr_node == NULL) {
            Node* newNode = createNode();
            newNode->prev = tmp;
            tmp->next = newNode;
            curr_node = newNode;
            break;
        }
    }
    curr_node->sections[curr_node->count] = section;
    curr_node->count++;
}

int countSections(Node* head) {
    Node* curr_node = head;
    int count = 0;
    do {
        count += (curr_node->count - curr_node->deleted);
        curr_node = curr_node->next;
    } while (curr_node != NULL);
    return count;
}

void deleteNode(Node** head, Node* nodeToDelete) {
    if (*head == NULL || nodeToDelete == NULL) {
        return;
    }

    if (*head == nodeToDelete) {
        *head = nodeToDelete->next;
    }

    if (nodeToDelete->next != NULL) {
        nodeToDelete->next->prev = nodeToDelete->prev;
    }

    if (nodeToDelete->prev != NULL) {
        nodeToDelete->prev->next = nodeToDelete->next;
    }

    Section* section;
    Selector* selector;
    Attribute* attribute;

    for (int i = 0; i < T; i++) {
        section = nodeToDelete->sections[i];

        if (section != NULL) {
            selector = section->selectors;
            attribute = section->attributes;

            while (selector != NULL) {
                Selector* nextSelector = selector->next;
                free(selector->name);
                free(selector);
                selector = nextSelector;
            }

            while (attribute != NULL) {
                Attribute* nextAttribute = attribute->next;
                free(attribute->name);
                free(attribute->value);
                free(attribute);
                attribute = nextAttribute;
            }

            free(section);
        }
    }

    free(nodeToDelete);
    if (*head == NULL) {
        *head = createNode();
    }
}


void deleteSection(Node** node, int index, Node** tail) {
    Node* curr_node = *node;
    if (index > countSections(curr_node)) return;
    int number = index - 1;
    while (curr_node->next != NULL) {
        if (number < (curr_node->count - curr_node->deleted)) break;
        number -= (curr_node->count - curr_node->deleted);
        curr_node = curr_node->next;
    }
    Section* section = curr_node->sections[number];
    if (section->countAttributes != 0) {
        Attribute* attribute = section->attributes;
        while (attribute != NULL && attribute->name != NULL && attribute->value == NULL) {
            Attribute* nextAttribute = attribute->next;
            deleteAttribute(section, attribute);
            attribute = nextAttribute;
        }
    }
    if (section->countSelectors != 0) {
        Selector* selector = section->selectors;
        while (selector != NULL) {
            Selector* nextSelector = selector->next;
            deleteSelector(section, selector);
            selector = nextSelector;
        }
    }
    free(section);
    for (int i = number; i < curr_node->count - 1; i++) {
        curr_node->sections[i] = curr_node->sections[i + 1];
    }
    curr_node->sections[curr_node->count - 1] = NULL;
    if (curr_node->next == NULL || curr_node->next->count == 0) {
        curr_node->count--;
    }
    else
        curr_node->deleted++;
    if (curr_node->count - curr_node->deleted == 0) {
        if (*tail == *node) *tail = (*tail)->prev;
        deleteNode(node, curr_node);
    }
}

// sprawdzenie, czy podany ciąg znaków jest liczbą
bool ifInt(char* numArray)
{
    for (int i = 0; i < strlen(numArray); i++) {
        if (numArray[i] < '0' || numArray[i] > '9')
            return false;
    }
    return true;
}

// zamiana ciągu znaków na liczbę
int str_to_int(const char* numArray) {
    int num = 0;
    for (int i = 0; i < strlen(numArray); i++, num *= 10) {
        num += numArray[i] - '0';
    }
    return num / 10;
}

// usuwanie niepotrzebnych znaków z początku i końca ciągu znaków
void trim_whitespace(char* str) {
    int len = strlen(str);
    int start = 0;
    while (len > 0 && (str[len - 1] <= ' ' || str[len - 1] == '}' || str[len - 1] == '{' || str[len - 1] == ':' || str[len - 1] == ';' || str[len - 1] == '\n' || str[len - 1] == '\t' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
    while (str[start] <= ' ' || str[start] == '}' || str[start] == '{' || str[start] == ':' || str[start] == '\n' || str[start] == '\t' || str[start] == '\r') {
        start++;
    }
    memmove(str, str + start, len - start + 1);
}

// sprawdzenie, czy podany selektor już istnieje w sekcji
bool selector_exist(Section* current_section, char* name) {
    Selector* current_selector = current_section->selectors;
    if (current_selector == NULL) return false;
    for (int i = 1; i <= current_section->countSelectors; i++) {
        if (!strcmp(current_selector->name, name))
            return true;
        current_selector = current_selector->next;
    }
    return false;
}

// wykonywanie komend
void do_commands(Node** head, Node **tail, char* line) {
    if (!strcmp(line, "") || !strcmp(line, "\n") || !strcmp(line, "\t")) return;
    if (!strcmp(line, "?")) {
        printf("? == %d\n", countSections(*head));
    }
    else {
        char* first_parammetr = strtok(line, ",");
        char* second_parammetr = strtok(NULL, ",");
        char* third_parammetr = strtok(NULL, ",");
        if (first_parammetr == NULL || second_parammetr == NULL || third_parammetr == NULL) {
            return;
        }
        Node* curr_node = *head;
        Selector* curr_selector = NULL;
        Attribute* curr_attribute = NULL;
        if (!strcmp(second_parammetr, "S") && ifInt(first_parammetr) && !strcmp(third_parammetr, "?")) {
            if (str_to_int(first_parammetr) > countSections(*head)) return;
            int number = str_to_int(first_parammetr) - 1;
            while (curr_node->next != NULL) {
                if (number < (curr_node->count - curr_node->deleted)) break;
                number -= (curr_node->count - curr_node->deleted);
                curr_node = curr_node->next;
            }
            printf("%s,S,? == %d\n", first_parammetr, curr_node->sections[number]->countSelectors);
        }
        if (!strcmp(second_parammetr, "S") && ifInt(first_parammetr) && ifInt(third_parammetr)) {
            if (str_to_int(first_parammetr) > countSections(*head)) return;
            int number = str_to_int(first_parammetr) - 1;
            while (curr_node->next != NULL) {
                if (number < (curr_node->count - curr_node->deleted)) break;
                number -= (curr_node->count - curr_node->deleted);
                curr_node = curr_node->next;
            }
            curr_selector = curr_node->sections[number]->selectors;
            for (int i = 1; i <= curr_node->sections[number]->countSelectors; i++) {
                if (i == str_to_int(third_parammetr)) {
                    printf("%s,S,%s == %s\n", first_parammetr, third_parammetr, curr_selector->name);
                    break;
                }
                curr_selector = curr_selector->next;
            }
        }
        if (!strcmp(second_parammetr, "S") && !ifInt(first_parammetr) && !strcmp(third_parammetr, "?")) {
            int count = 0;
            while (curr_node != NULL) {
                for (int i = 0; i < (curr_node->count - curr_node->deleted); i++) {
                    curr_selector = curr_node->sections[i]->selectors;
                    for (int j = 1; j <= curr_node->sections[i]->countSelectors; j++) {
                        if (!strcmp(curr_selector->name, first_parammetr)) {
                            count++;
                            break;
                        }
                        curr_selector = curr_selector->next;
                    }
                }
                curr_node = curr_node->next;
            }
            printf("%s,S,? == %d\n", first_parammetr, count);
        }
        if (!strcmp(second_parammetr, "A") && ifInt(first_parammetr) && !strcmp(third_parammetr, "?")) {
            if (str_to_int(first_parammetr) > countSections(*head)) return;
            int number = str_to_int(first_parammetr) - 1;
            while (curr_node->next != NULL) {
                if (number < (curr_node->count - curr_node->deleted)) break;
                number -= (curr_node->count - curr_node->deleted);
                curr_node = curr_node->next;
            }
            printf("%s,A,? == %d\n", first_parammetr, curr_node->sections[number]->countAttributes);
        }
        if (!strcmp(second_parammetr, "A") && !ifInt(first_parammetr) && !strcmp(third_parammetr, "?")) {
            int count = 0;
            while (curr_node != NULL) {
                for (int i = 0; i < (curr_node->count - curr_node->deleted); i++) {
                    curr_attribute = curr_node->sections[i]->attributes;
                    for (int j = 1; j <= curr_node->sections[i]->countAttributes; j++) {
                        if (!strcmp(curr_attribute->name, first_parammetr)) {
                            count++;
                            break;
                        }
                        curr_attribute = curr_attribute->next;
                    }
                }
                curr_node = curr_node->next;
            }
            printf("%s,A,? == %d\n", first_parammetr, count);
        }
        if (!strcmp(second_parammetr, "A") && ifInt(first_parammetr) && strcmp(third_parammetr, "?")) {
            if (str_to_int(first_parammetr) > countSections(*head)) return;
            int number = str_to_int(first_parammetr) - 1;
            while (curr_node->next != NULL) {
                if (number < (curr_node->count - curr_node->deleted)) break;
                number -= (curr_node->count - curr_node->deleted);
                curr_node = curr_node->next;
            }
            curr_attribute = curr_node->sections[number]->attributes;
            for (int i = 1; i <= curr_node->sections[number]->countAttributes; i++) {
                if (!strcmp(curr_attribute->name, third_parammetr)) {
                    printf("%s,A,%s == %s\n", first_parammetr, third_parammetr, curr_attribute->value);
                    break;
                }
                curr_attribute = curr_attribute->next;
            }
        }
        if (!strcmp(second_parammetr, "E")) {
            curr_node = *tail;
            bool sel = false, atr = false;
            while (curr_node != NULL) {
                for (int i = (curr_node->count - curr_node->deleted) - 1; i >= 0; i--) {
                    curr_selector = curr_node->sections[i]->selectors;
                    curr_attribute = curr_node->sections[i]->attributes;
                    for (int j = 1; j <= curr_node->sections[i]->countSelectors; j++) {
                        if (!strcmp(curr_selector->name, first_parammetr)) {
                            sel = true;
                            break;
                        }
                        curr_selector = curr_selector->next;
                    }
                    if (!sel) continue;
                    for (int j = 1; j <= curr_node->sections[i]->countAttributes; j++) {
                        if (!strcmp(curr_attribute->name, third_parammetr)) {
                            atr = true;
                            break;
                        }
                        curr_attribute = curr_attribute->next;
                    }
                    if (sel && atr) break;
                }
                if (sel && atr) break;
                curr_node = curr_node->prev;
            }
            if (sel && atr && curr_attribute != NULL) {
                printf("%s,E,%s == %s\n", first_parammetr, third_parammetr, curr_attribute->value);
            }
        }
        if (!strcmp(second_parammetr, "D") && !strcmp(third_parammetr, "*")) {
            int count = countSections(*head);
            deleteSection(head, str_to_int(first_parammetr), tail);
            if (countSections(*head) < count) printf("%s,D,* == deleted\n", first_parammetr);
        }
        if (!strcmp(second_parammetr, "D") && strcmp(third_parammetr, "*")) {
            if (str_to_int(first_parammetr) > countSections(*head)) return;
            int number = str_to_int(first_parammetr) - 1;
            while (curr_node->next != NULL) {
                if (number < (curr_node->count - curr_node->deleted)) break;
                number -= (curr_node->count - curr_node->deleted);
                curr_node = curr_node->next;
            }
            curr_attribute = curr_node->sections[number]->attributes;
            for (int i = 1; i <= curr_node->sections[number]->countAttributes; i++) {
                if (!strcmp(curr_attribute->name, third_parammetr)) {
                    deleteAttribute(curr_node->sections[number], curr_attribute);
                    printf("%s,D,%s == deleted\n", first_parammetr, third_parammetr);
                    break;
                }
                curr_attribute = curr_attribute->next;
            }
            if (curr_node->sections[number]->countAttributes == 0) {
                deleteSection(head, str_to_int(first_parammetr), tail);
            }
        }
    }
}

// pobranie i parsowanie css
void load_css(Node** head) {
    Node* current_node = *head;
    int state = DATA;

    int current_index = 0;
    char line[STRING_SIZE];
    char backup[SECTION_SIZE] = "start of the section";
    while (fgets(line, sizeof(line) / sizeof(char), stdin)) {
        if (!strcmp(line, " ") || !strcmp(line, "\n") || !strcmp(line, "\t")) continue;
        if (line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\t')
            line[strlen(line) - 1] = '\0';
        if (!strcmp(line, "????")) {
            state = COMMAND;
        }
        else if (!strcmp(line, "****")) {
            state = DATA;
            if ((*head)->count == 0) {
                current_node = *head;
                current_index = 0;
                continue;
            }
            if (current_node->count == T)
                current_index = 0;
            else
                current_index = current_node->count;
        }
        else if (state == DATA) {
            size_t len = strlen(line);
            while (len > 0 && (line[len - 1] == '\r' || line[len - 1] == '\n' || line[len - 1] == ' ' || line[len - 1] == '\t')) {
                line[len - 1] = '\0';
                len--;
            }
            // wczytywanie linijek, aż w jednym char array nie będzie całej sekcji
            if (strchr(line, '}') == NULL || (strchr(line, '}') != NULL && len > 1)) {
                if (strcmp(backup, "start of the section")) {
                    strcat(backup, line);
                }
                else {
                    strcpy(backup, line);
                }
                if (!(strchr(line, '}') != NULL && len > 1)) {
                    continue;
                }
            }
            addSection(head, createSection());
            if (current_node->count == T && current_node->sections[current_index]->countSelectors != 0) {
                current_node = current_node->next;
            }
            // Usuń klamrę otwierającą
            char* backup_attributes;
            char* section_name = strtok(backup, "{");
            backup_attributes = strtok(NULL, "{");
            if (backup_attributes != NULL) {
                char* selector_names = strtok(section_name, ",");
                while (selector_names != NULL) {
                    // Dodaj selektor do sekcji
                    trim_whitespace(selector_names);
                    if (!selector_exist(current_node->sections[current_index], selector_names)) {
                        addSelector(current_node->sections[current_index], createSelector(selector_names));
                    }

                    // Przetwórz następny selektor
                    selector_names = strtok(NULL, ",");
                }
            }
            bool global = false;
            if (backup_attributes == NULL) {
                global = true;
                backup_attributes = _strdup(section_name);
            }
            char delimiter[] = ";";
            char* token = strtok(backup_attributes, delimiter);
            // Dodaj atrybut do bieżącego selektora
            while (token != NULL) {
                // znajdź pozycję dwukropka
                char* colon = strchr(token, ':');
                if (colon != NULL) {
                    // wydziel nazwę i wartość atrybutu
                    *colon = '\0'; // zamień dwukropek na znak końca łańcucha
                    char* name = token;
                    char* value = colon + 1;
                    // usuń początkowe i końcowe białe znaki z nazwy i wartości atrybutu
                    trim_whitespace(name);
                    trim_whitespace(value);
                    if (current_node->sections[current_index] != NULL) {
                        Attribute* curr_attribute = current_node->sections[current_index]->attributes;
                        while (curr_attribute != NULL) {
                            if (!strcmp(curr_attribute->name, name)) {
                                deleteAttribute(current_node->sections[current_index], curr_attribute);
                                break;
                            }
                            curr_attribute = curr_attribute->next;
                        }
                        addAttribute(current_node->sections[current_index], createAttribute(name, value));
                    }
                }
                token = strtok(NULL, delimiter);
            }
            if (global) free(backup_attributes);
            memset(&backup[0], 0, sizeof(backup));
            strcpy(backup, "start of the section");
            current_index++;
            current_index %= T;
        }
        else if (state == COMMAND) {
            do_commands(head, &current_node, line);
        }
    }
}

int main() {
    Node* node = createNode();
    load_css(&node);
    return 0;
}