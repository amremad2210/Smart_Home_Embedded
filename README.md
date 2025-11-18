
# Door Locker System – TM4C123 (Control & HMI ECUs)

This repository contains the embedded software for a **door locker system**
implemented on **two TM4C123 (Tiva C) microcontrollers**:

- **Control_WS** – Control ECU (motor, buzzer, EEPROM, logic)
- **HIMI_WS** – HMI ECU (keypad, LCD, LEDs, user interface)

Both projects are managed in a single IAR Embedded Workbench workspace.

---

## 1. Repository structure

```text
Project_WS/
├─ Control_WS/
│  ├─ main.c              # Application entry point (Control ECU)
│  ├─ inc/                # Headers (types, macros, MCU header, etc.)
│  ├─ src/                # Drivers & modules (will be added gradually)
│  ├─ Debug/              # IAR build output (ignored by git)
│  └─ settings/           # IAR debug settings (ignored by git)
│
├─ Himi_WS/
│  ├─ main.c              # Application entry point (HMI ECU)
│  ├─ inc/
│  ├─ src/
│  ├─ Debug/
│  └─ settings/
│
├─ Smart_Home_WS.eww      # IAR workspace with both projects
├─ .gitignore             # ignores builds, temp files, etc.
└─ README.md
````

> **Rule of thumb**
>
> * Put `.c` files in `src/` **(except main.c which stays at project root)**
> * Put `.h` files in `inc/`
> * Never commit anything from `Debug/` or `settings/`.

---

## 2. Tools / requirements

* **IAR Embedded Workbench for ARM** (same major version for all team members)
* **Git** (command line or any GUI client)
* TM4C123G LaunchPad (or equivalent board) for each ECU

---

## 3. How to clone and open the project

### 3.1 Clone the repository (first time)

```bash
# choose a folder to keep your projects, then:
git clone https://github.com/<your-user>/<your-repo>.git
cd <your-repo>
```

> Replace `https://github.com/<your-user>/<your-repo>.git` with the real URL.

### 3.2 Open in IAR

1. Start **IAR Embedded Workbench**.
2. `File` → `Open` → `Workspace…`
3. Open: `Smart_Home_WS.eww`
4. You will see two projects in the workspace:

   * `Control_WS - Debug`
   * `HIMI_WS - Debug`

### 3.3 Build each ECU

1. Select the project you want as **Active** (right-click → *Set as Active*).
2. Press **Build** (or `F7`).
3. Repeat for the other project.

If include paths are correct, both should build without errors.

---

## 4. Git workflow – how to work safely

To avoid conflicts and confusion, we follow this simple workflow:

### 4.1 Branches

* `main`

  * Always **stable**, **builds successfully**.
  * No one commits directly here (ideally protected by GitHub).
* Feature branches

  * For example:

    * `feature/control-uart`
    * `feature/hmi-keypad`
    * `bugfix/control-timer-overflow`

### 4.2 Before you start working

Always sync with the remote repo first:

```bash
git checkout main
git pull
```

Then create a feature branch from the updated `main`:

```bash
git checkout -b feature/<short-description>
```

Examples:

```bash
git checkout -b feature/hmi-lcd-driver
git checkout -b feature/control-motor-pwm
```

Now do your edits in IAR on this branch.

### 4.3 While working

1. Check what changed:

   ```bash
   git status
   ```

2. Stage and commit logically (small, focused commits):

   ```bash
   git add .
   git commit -m "Implement basic LCD init and clear functions"
   ```

3. Push your branch to GitHub:

   ```bash
   git push -u origin feature/hmi-lcd-driver
   ```

### 4.4 Creating a Pull Request (PR)

1. Go to GitHub.
2. You will see a suggestion to open a PR from your branch.
3. Create a PR into `main`.
4. Another teammate reviews the code, then merges it when:

   * It builds,
   * It does not break other features.

After merge, you can delete the feature branch.

---

## 5. Updating your local repo after others merge

When someone else merges to `main`:

```bash
# go to main and update
git checkout main
git pull
```

If you are currently on a feature branch and want the latest changes:

```bash
git checkout main
git pull
git checkout feature/your-branch
git merge main
# resolve any conflicts (if they exist), then:
git add .
git commit -m "Merge main into feature/your-branch"
```

---

## 6. Files we track vs ignore

**Tracked (kept in Git):**

* `*.c`, `*.h` files in `Control_WS` and `Himi_WS`
* Workspace & project files: `Smart_Home_WS.eww`, `*.ewp`
* Docs: `README.md`, any PDFs or design documents you add

**Ignored (auto-generated, see `.gitignore`):**

* `Debug/` folders
* `settings/` folders
* IAR temp/build files: `*.dep`, `*.ewd`, `*.ewt`, `*.pbd`, `*.out`, `*.hex`, `*.map`, `*.o`, etc.

---

## 7. Coding / project conventions

* `main.c` stays at the root of each ECU project (`Control_WS/main.c`, `Himi_WS/main.c`).
* All other modules go under `src/`, headers under `inc/`.
* Common type & macro headers:

  * `Types.h`
  * `common_macros.h`
  * `tm4c123gh6pm.h`

Try to keep modules small and focused (e.g. `motor.c`, `buzzer.c`, `lcd.c`).

---


```


```
