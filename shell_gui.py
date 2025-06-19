import tkinter as tk
from tkinter.scrolledtext import ScrolledText
import subprocess
import threading
import time
import re

def remove_ansi(text):
    ansi_escape = re.compile(r'\x1B[@-_][0-?]*[ -/]*[@-~]')
    return ansi_escape.sub('', text)

class MiniShellGUI:
    def __init__(self, root):
        self.root = root
        root.title("Minishell")

        self.input_label = tk.Label(root, text="➜", font=("Consolas", 16), fg="#00f0ff", bg="#1e1e2e")
        self.input_label.grid(row=0, column=0, padx=5, pady=5)

        self.input_entry = tk.Entry(root, font=("Consolas", 16), bg="#2e2e3e", fg="white", insertbackground="white", width=60)
        self.input_entry.grid(row=0, column=1, padx=5, pady=5)
        self.input_entry.bind("<Return>", self.run_command)

        self.run_button = tk.Button(root, text="Run", font=("Consolas", 14), bg="#55ccff", command=self.run_command)
        self.run_button.grid(row=0, column=2, padx=5, pady=5)

        self.output_text = ScrolledText(root, font=("Consolas", 12), bg="#1e1e2e", fg="white", state="disabled", width=100, height=25)
        self.output_text.grid(row=1, column=0, columnspan=3, padx=5, pady=5)

        self.write_output("[Welcome to Minishell!!]\n")

        # 🔴 Start one shell process on GUI launch
        self.shell = subprocess.Popen(
            ["wsl", "./shell"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1
        )

        # Start thread to read output continuously
        threading.Thread(target=self.read_output, daemon=True).start()

    def write_output(self, text):
        self.output_text.config(state="normal")
        self.output_text.insert(tk.END, f"[{time.strftime('%H:%M:%S')}] {remove_ansi(text)}")
        self.output_text.see(tk.END)
        self.output_text.config(state="disabled")

    def run_command(self, event=None):
        command = self.input_entry.get().strip()
        self.input_entry.delete(0, tk.END)
        if command == "":
            return
        try:
            self.shell.stdin.write(command + "\n")
            self.shell.stdin.flush()
        except Exception as e:
            self.write_output(f"Error writing to shell: {e}\n")

    def read_output(self):
        try:
            while True:
                output = self.shell.stdout.readline()
                if output == '':
                    break
                self.write_output(output)
        except Exception as e:
            self.write_output(f"Error reading output: {e}\n")

def main():
    root = tk.Tk()
    root.configure(bg="#1e1e2e")
    app = MiniShellGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
