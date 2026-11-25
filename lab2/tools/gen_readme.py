#!/usr/bin/env python3
"""Generates lab2/README.md from the headers so that counts never drift from the code."""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
HEADERS = sorted((ROOT / "include" / "hotel").glob("*.hpp"))
SKIP = {"constants.hpp", "exceptions.hpp", "validation.hpp"}
CLASS_RE = re.compile(r"^class (\w+)(?:\s*:\s*public\s+(\w+))?\s*\{")
FIELD_RE = re.compile(r"^\s{4}[\w:<>, *&]+?\s+(\w+_);$")
METHOD_RE = re.compile(r"(\w+)\(")


def parse_classes():
    classes = []
    for header in HEADERS:
        if header.name in SKIP:
            continue
        current, access = None, None
        for line in header.read_text(encoding="utf-8").splitlines():
            match = CLASS_RE.match(line)
            if match:
                current = {"name": match.group(1), "base": match.group(2), "module": header.stem,
                           "fields": [], "methods": [], "text": ""}
                access = "private"
                continue
            if current is None:
                continue
            if line.startswith("};"):
                classes.append(current)
                current = None
                continue
            current["text"] += line + "\n"
            stripped = line.strip()
            if stripped in ("public:", "private:", "protected:"):
                access = stripped[:-1]
                continue
            collect_member(current, access, line)
    return classes


def collect_member(current, access, line):
    field = FIELD_RE.match(line)
    if field and access != "public":
        current["fields"].append(field.group(1).rstrip("_"))
        return
    if access != "public" or not line.rstrip().endswith(";") or "operator" in line:
        return
    name = METHOD_RE.search(line)
    if name and name.group(1) != current["name"]:
        current["methods"].append(name.group(1))


def parse_exceptions():
    text = (ROOT / "include" / "hotel" / "exceptions.hpp").read_text(encoding="utf-8")
    return re.findall(r"^class (\w+) : public (\w+)", text, re.MULTILINE)


def associations(cls, names):
    found = [n for n in names if n != cls["name"] and n != cls["base"] and re.search(rf"\b{n}\b", cls["text"])]
    return sorted(found)


def render(classes, exceptions):
    names = [c["name"] for c in classes]
    for cls in classes:
        cls["assoc"] = associations(cls, names)
    total_fields = sum(len(c["fields"]) for c in classes)
    total_methods = sum(len(c["methods"]) for c in classes)
    total_assoc = sum(len(c["assoc"]) for c in classes)
    out = [HEAD.format(classes=len(classes), fields=total_fields, methods=total_methods,
                       assoc=total_assoc, exceptions=len(exceptions))]
    out.append("| Класс | Поля | Методы | Ассоциации (связанные классы) |\n|---|---|---|---|")
    for cls in sorted(classes, key=lambda c: c["name"]):
        out.append(f"| {cls['name']} | {len(cls['fields'])} | {len(cls['methods'])} | {', '.join(cls['assoc']) or '—'} |")
    out.append("\n## Подробное описание классов\n")
    for cls in classes:
        base = f" (наследует `{cls['base']}`)" if cls["base"] else ""
        out.append(f"### {cls['name']}{base}\n- модуль: `{cls['module']}`\n"
                   f"- поля: {', '.join('`'+f+'`' for f in cls['fields']) or '—'}\n"
                   f"- методы: {', '.join('`'+m+'`' for m in cls['methods']) or '—'}\n"
                   f"- ассоциации: {', '.join(cls['assoc']) or '—'}\n")
    out.append("## Исключения\n")
    out.extend(f"- `{name}` ← `{base}`" for name, base in exceptions)
    print(f"classes={len(classes)} fields={total_fields} behaviors={total_methods} "
          f"associations={total_assoc} exceptions={len(exceptions)}")
    return "\n".join(out) + "\n"


HEAD = """# Лабораторная работа №2 — «Гостиница»

Крупный ООП-проект на C++20: модель гостиницы (номера, бронирование, заселение и
выселение, счета и платежи, персонал, ресторан, парковка, спа, конференц-залы).

## Сборка, тесты, запуск

```bash
make            # сборка консольного приложения -> build/hotel_cli
make test       # сборка и запуск unit-тестов
./build/hotel_cli
```

Или через CMake:

```bash
cmake -S . -B build-cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build-cmake
ctest --test-dir build-cmake --output-on-failure
```

## Структура

- `include/hotel/*.hpp` — объявления классов (интерфейс), `src/*.cpp` — реализации;
- `cli/` — консольное меню (`ConsoleMenu`), отделено от предметной области и принимает
  потоки ввода/вывода, поэтому покрывается тестами; `app/main.cpp` — точка входа;
- `tests/` — unit-тесты на собственном мини-фреймворке (без внешних зависимостей);
- `tools/gen_readme.py` — генератор этого файла (цифры считаются из заголовков).

## Соответствие требованиям

| Требование | Нужно | Есть |
|---|---|---|
| Классы | ≥ 50 | {classes} |
| Поля | ≥ 150 | {fields} |
| Поведения (методы с логикой, без get/set и inline-доступа) | ≥ 100 | {methods} |
| Ассоциации (класс как поле или параметр метода) | ≥ 30 | {assoc} |
| Собственные исключения | ≥ 12 | {exceptions} |

## Сводная таблица классов

Формат: `Класс | число полей | число методов | связанные классы`.

"""

if __name__ == "__main__":
    (ROOT / "README.md").write_text(render(parse_classes(), parse_exceptions()), encoding="utf-8")
