#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
using namespace std;

const int MAX_ACCOUNTS = 3;

string money(double amount) {
    ostringstream out;
    out << fixed << setprecision(2) << amount;
    string digits = out.str();
    size_t dot = digits.find('.');
    string whole = digits.substr(0, dot);
    string cents = digits.substr(dot);
    string result = "";
    int count = 0;
    for (int i = (int)whole.size() - 1; i >= 0; i--) {
        result = whole[i] + result;
        count++;
        if (count % 3 == 0 && i > 0 && whole[i - 1] != '-') result = "," + result;
    }
    return "$" + result + cents;
}

string escapeHtml(const string& text) {
    string safe = "";
    for (char c : text) {
        if (c == '&') safe += "&amp;";
        else if (c == '<') safe += "&lt;";
        else if (c == '>') safe += "&gt;";
        else if (c == '"') safe += "&quot;";
        else safe += c;
    }
    return safe;
}

double readNumber(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Please type a number, like 250.50\n";
    }
}

class BankAccount {
private:
    string name;
    double balance;
    double totalDeposited;
    int depositCount;
    int withdrawCount;

public:
    BankAccount(string n, double startBalance) {
        name = n;
        balance = startBalance;
        totalDeposited = 0;
        depositCount = 0;
        withdrawCount = 0;
    }

    bool deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            totalDeposited += amount;
            depositCount++;
            return true;
        }
        return false;
    }

    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            withdrawCount++;
            return true;
        }
        return false;
    }

    string getName() const { return name; }
    double getBalance() const { return balance; }
    int getDepositCount() const { return depositCount; }
    int getWithdrawCount() const { return withdrawCount; }

    string getStatus() const {
        if (balance >= 5000) return "Premium - Excellent";
        else if (balance >= 1000) return "Good Standing";
        else return "Needs Improvement";
    }

    string getTierClass() const {
        if (balance >= 5000) return "premium";
        else if (balance >= 1000) return "good";
        else return "needs";
    }

    void show() const {
        cout << "  Name:            " << name << "\n";
        cout << "  Balance:         " << money(balance) << "\n";
        cout << "  Deposits made:   " << depositCount << "\n";
        cout << "  Total deposited: " << money(totalDeposited) << "\n";
        cout << "  Status:          " << getStatus() << "\n";
    }

    string toHtml() const {
        ostringstream h;
        h << fixed << setprecision(2);
        h << "      <article class=\"card " << getTierClass() << "\""
          << " data-name=\"" << escapeHtml(name) << "\""
          << " data-balance=\"" << balance << "\""
          << " data-deposited=\"" << totalDeposited << "\""
          << " data-deposits=\"" << depositCount << "\""
          << " data-withdrawals=\"" << withdrawCount << "\">\n"
          << "        <h2>" << escapeHtml(name) << "</h2>\n"
          << "        <p>" << money(balance) << " - " << getStatus() << "</p>\n"
          << "      </article>\n";
        return h.str();
    }
};

const char* kHead = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Bank Account Simulator</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Bricolage+Grotesque:wght@600;800&family=Figtree:wght@400;600&display=swap" rel="stylesheet">
<style>
:root{--ink:#0f1b33;--card:#17274b;--paper:#eef1f6;--text:#1c2742;--muted:#5d6a86;
--premium:#f2c45a;--good:#33c9b8;--needs:#ff8271}
*{box-sizing:border-box}
[hidden]{display:none!important}
body{margin:0;background:var(--paper);color:var(--text);font:16px/1.5 'Figtree',system-ui,sans-serif}
.wrap{max-width:1080px;margin:0 auto;padding:0 20px}
.hero{background:var(--ink);color:#fff;padding:40px 0}
.hero h1{margin:0;font:600 18px/1.3 'Figtree',system-ui,sans-serif;color:#b9c4de}
.total{margin:6px 0 28px;font:800 clamp(48px,12vw,120px)/1 'Bricolage Grotesque',system-ui,sans-serif;
letter-spacing:-.03em;font-variant-numeric:tabular-nums}
.stats{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:14px 28px;margin:0;
padding-top:18px;border-top:1px solid #2c3d66}
.stats dt{color:#9fb0d3;font-size:14px}
.stats dd{margin:2px 0 0;font:600 22px 'Bricolage Grotesque',system-ui,sans-serif;font-variant-numeric:tabular-nums}
main{padding:34px 0 48px}
.title{margin:0 0 4px;font:800 26px 'Bricolage Grotesque',system-ui,sans-serif}
.lede{margin:0 0 22px;color:var(--muted);max-width:60ch}
.cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:20px}
.card{--accent:var(--needs);background:var(--card);color:#fff;border-radius:20px;padding:22px;
border-top:6px solid var(--accent);box-shadow:0 18px 30px -20px rgba(15,27,51,.8)}
.card.good{--accent:var(--good)}
.card.premium{--accent:var(--premium)}
.top{display:flex;justify-content:space-between;align-items:center;margin-bottom:18px}
.chip{width:40px;height:30px;border-radius:7px;background:var(--accent);
background-image:linear-gradient(90deg,transparent 48%,rgba(15,27,51,.35) 48% 52%,transparent 52%),
linear-gradient(0deg,transparent 48%,rgba(15,27,51,.35) 48% 52%,transparent 52%)}
.badge{background:var(--accent);color:var(--ink);font-weight:600;font-size:13px;padding:4px 12px;border-radius:999px}
.card h2{margin:0;font:600 20px 'Bricolage Grotesque',system-ui,sans-serif}
.bal{margin:2px 0 14px;font:800 40px/1.1 'Bricolage Grotesque',system-ui,sans-serif;font-variant-numeric:tabular-nums}
.meter{height:8px;border-radius:99px;background:#0f1b33;overflow:hidden}
.meter span{display:block;height:100%;background:var(--accent);border-radius:99px;transition:width .8s cubic-bezier(.2,.8,.2,1)}
.hint{margin:6px 0 16px;color:#b9c4de;font-size:14px}
.mini{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;margin:0 0 18px}
.mini dt{color:#9fb0d3;font-size:12px}
.mini dd{margin:0;font-weight:600;font-variant-numeric:tabular-nums}
.row{display:flex;gap:8px}
.amt{flex:1;min-width:0;display:flex;align-items:center;gap:6px;background:var(--ink);border:1px solid #2c3d66;
border-radius:12px;padding:0 12px}
.amt input{flex:1;min-width:0;background:none;border:0;color:#fff;font:inherit;padding:10px 0}
.amt input:focus{outline:none}
.amt:focus-within{border-color:var(--accent)}
button{font:600 15px 'Figtree',system-ui,sans-serif;border-radius:12px;padding:10px 14px;cursor:pointer}
.dep{background:var(--accent);color:var(--ink);border:0}
.wd{background:none;color:#fff;border:1px solid #4a5e8f}
button:focus-visible,input:focus-visible{outline:2px solid #fff;outline-offset:2px}
button:active{transform:translateY(1px)}
.msg{min-height:22px;margin:10px 0 4px;font-size:14px;color:var(--accent)}
.log{list-style:none;margin:0;padding:0;font-size:14px;color:#b9c4de}
.log li{display:flex;justify-content:space-between;padding:5px 0;border-top:1px solid #2c3d66}
.add{margin-top:24px;background:#fff;border-radius:20px;padding:22px;border:1px solid #d6dcea}
.add h3{margin:0 0 12px;font:800 20px 'Bricolage Grotesque',system-ui,sans-serif}
.fields{display:flex;flex-wrap:wrap;gap:10px}
.fields input{flex:1 1 180px;min-width:0;font:inherit;padding:10px 12px;border:1px solid #b9c2d8;border-radius:12px;background:#fff;color:var(--text)}
.fields button{background:var(--ink);color:#fff;border:0}
.add .err{min-height:22px;margin:8px 0 0;font-size:14px;color:#b42318}
.full{margin-top:24px;color:var(--muted)}
@media (prefers-reduced-motion:reduce){.meter span{transition:none}}
</style>
</head>
<body>
)HTML";

const char* kScript = R"HTML(
<script>
(() => {
  const MAX = 3;
  const money = n => n.toLocaleString('en-US', {style:'currency', currency:'USD'});
  const reduce = matchMedia('(prefers-reduced-motion: reduce)').matches;
  const esc = s => s.replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
  const grid = document.querySelector('.cards');

  function tierOf(b) {
    if (b >= 5000) return {key:'premium', rank:2, label:'Premium - Excellent', next:null};
    if (b >= 1000) return {key:'good', rank:1, label:'Good Standing', next:5000, nextLabel:'Premium'};
    return {key:'needs', rank:0, label:'Needs Improvement', next:1000, nextLabel:'Good Standing'};
  }

  const cards = [...document.querySelectorAll('.card')].map(el => ({
    el, name: el.dataset.name, balance: +el.dataset.balance, deposited: +el.dataset.deposited,
    deposits: +el.dataset.deposits, withdrawals: +el.dataset.withdrawals, log: [], msg: '', pct: 0
  }));

  function render(c) {
    const t = tierOf(c.balance);
    const pct = t.next ? Math.min(100, c.balance / t.next * 100) : 100;
    const hint = t.next ? money(t.next - c.balance) + ' to reach ' + t.nextLabel : 'Top tier reached';
    c.el.className = 'card ' + t.key;
    c.el.innerHTML = `
      <div class="top"><span class="chip" aria-hidden="true"></span><span class="badge">${t.label}</span></div>
      <h2>${esc(c.name)}</h2>
      <p class="bal">${money(c.balance)}</p>
      <div class="meter" role="progressbar" aria-label="Progress to next tier" aria-valuemin="0" aria-valuemax="100" aria-valuenow="${Math.round(pct)}"><span style="width:${c.pct}%"></span></div>
      <p class="hint">${hint}</p>
      <dl class="mini">
        <div><dt>Deposits</dt><dd>${c.deposits}</dd></div>
        <div><dt>Withdrawals</dt><dd>${c.withdrawals}</dd></div>
        <div><dt>Total deposited</dt><dd>${money(c.deposited)}</dd></div>
      </dl>
      <div class="row">
        <label class="amt"><span>$</span><input type="number" min="0" step="0.01" inputmode="decimal" placeholder="Amount" aria-label="Amount for ${esc(c.name)}"></label>
        <button class="dep" data-act="deposit">Deposit</button>
        <button class="wd" data-act="withdraw">Withdraw</button>
      </div>
      <p class="msg" role="status">${c.msg}</p>
      <ul class="log">${c.log.slice(0, 4).map(e => `<li><span>${e.type}</span><span>${e.type === 'Deposit' ? '+' : '-'}${money(e.amount)}</span></li>`).join('')}</ul>`;
    const bar = c.el.querySelector('.meter span');
    requestAnimationFrame(() => requestAnimationFrame(() => { bar.style.width = pct + '%'; }));
    c.pct = pct;
  }

  const shown = {};
  function setNumber(id, value) {
    const el = document.getElementById(id);
    const from = shown[id] ?? value;
    shown[id] = value;
    if (reduce || from === value) { el.textContent = money(value); return; }
    const start = performance.now();
    (function tick(now) {
      const p = Math.min(1, (now - start) / 700);
      el.textContent = money(from + (value - from) * (1 - Math.pow(1 - p, 3)));
      if (p < 1) requestAnimationFrame(tick);
    })(start);
  }

  function updateSummary() {
    const total = cards.reduce((s, c) => s + c.balance, 0);
    const top = cards.reduce((a, b) => (b.balance > a.balance ? b : a));
    setNumber('total', total);
    setNumber('avg', total / cards.length);
    document.getElementById('top').textContent = top.name;
    document.getElementById('tx').textContent = cards.reduce((s, c) => s + c.deposits + c.withdrawals, 0);
    document.getElementById('count').textContent = cards.length + ' of ' + MAX;
    const full = cards.length >= MAX;
    document.getElementById('add').hidden = full;
    document.getElementById('full').hidden = !full;
  }

  function confetti(el) {
    if (reduce) return;
    const r = el.getBoundingClientRect();
    const colors = ['#f2c45a', '#33c9b8', '#ff8271', '#ffffff'];
    for (let i = 0; i < 28; i++) {
      const p = document.createElement('span');
      p.style.cssText = `position:fixed;left:${r.left + r.width / 2}px;top:${r.top + 40}px;width:8px;height:12px;background:${colors[i % 4]};border-radius:2px;pointer-events:none;z-index:9`;
      document.body.appendChild(p);
      const dx = (Math.random() - .5) * 420, dy = 120 + Math.random() * 260;
      p.animate([{transform:'translate(0,0) rotate(0)', opacity:1},
                 {transform:`translate(${dx}px,${dy}px) rotate(${Math.random() * 720}deg)`, opacity:0}],
                {duration: 1100 + Math.random() * 600, easing:'cubic-bezier(.2,.7,.3,1)'}).onfinish = () => p.remove();
    }
  }

  function act(c, kind) {
    const input = c.el.querySelector('input');
    const amount = Math.round(parseFloat(input.value) * 100) / 100;
    const before = tierOf(c.balance);
    if (!(amount > 0)) {
      c.msg = 'Enter an amount above $0.';
    } else if (kind === 'withdraw' && amount > c.balance) {
      c.msg = 'Not enough funds. Balance is ' + money(c.balance) + '.';
    } else {
      if (kind === 'deposit') { c.balance += amount; c.deposited += amount; c.deposits++; }
      else { c.balance -= amount; c.withdrawals++; }
      c.balance = Math.round(c.balance * 100) / 100;
      c.log.unshift({type: kind === 'deposit' ? 'Deposit' : 'Withdrawal', amount});
      const after = tierOf(c.balance);
      c.msg = (kind === 'deposit' ? 'Deposited ' : 'Withdrew ') + money(amount) + '.';
      if (after.rank > before.rank) { c.msg = 'Moved up to ' + after.label + '!'; confetti(c.el); }
      else if (after.rank < before.rank) { c.msg = 'Dropped to ' + after.label + '.'; }
    }
    render(c);
    updateSummary();
    c.el.querySelector('input').focus();
  }

  document.getElementById('add').addEventListener('submit', e => {
    e.preventDefault();
    const nameInput = document.getElementById('newName');
    const balInput = document.getElementById('newBalance');
    const err = document.getElementById('addErr');
    const name = nameInput.value.trim();
    const bal = Math.round(parseFloat(balInput.value) * 100) / 100;
    if (!name || !(bal >= 0)) { err.textContent = 'Enter a name and a starting balance of $0 or more.'; return; }
    if (cards.length >= MAX) return;
    err.textContent = '';
    const el = document.createElement('article');
    grid.appendChild(el);
    const c = {el, name, balance: bal, deposited: 0, deposits: 0, withdrawals: 0, log: [], msg: 'Account added.', pct: 0};
    cards.push(c);
    render(c);
    updateSummary();
    nameInput.value = '';
    balInput.value = '';
    el.scrollIntoView({behavior: reduce ? 'auto' : 'smooth', block: 'nearest'});
  });

  cards.forEach(render);
  updateSummary();

  grid.addEventListener('click', e => {
    const btn = e.target.closest('button[data-act]');
    if (!btn) return;
    act(cards.find(c => c.el.contains(btn)), btn.dataset.act);
  });
  grid.addEventListener('keydown', e => {
    if (e.key === 'Enter' && e.target.matches('input')) act(cards.find(c => c.el.contains(e.target)), 'deposit');
  });
})();
</script>
</body>
</html>
)HTML";

void makeWebsite(const vector<BankAccount>& accounts, double total, double average) {
    ofstream file("index.html");
    if (!file) {
        cout << "Could not create index.html\n";
        return;
    }

    string topName = accounts[0].getName();
    double topBalance = accounts[0].getBalance();
    int transactions = 0;
    for (size_t i = 0; i < accounts.size(); i++) {
        if (accounts[i].getBalance() > topBalance) {
            topBalance = accounts[i].getBalance();
            topName = accounts[i].getName();
        }
        transactions += accounts[i].getDepositCount() + accounts[i].getWithdrawCount();
    }

    file << kHead;
    file << "<header class=\"hero\"><div class=\"wrap\">\n"
         << "  <h1>Total balance</h1>\n"
         << "  <p class=\"total\" id=\"total\">" << money(total) << "</p>\n"
         << "  <dl class=\"stats\">\n"
         << "    <div><dt>Accounts</dt><dd id=\"count\">" << accounts.size() << " of " << MAX_ACCOUNTS << "</dd></div>\n"
         << "    <div><dt>Average balance</dt><dd id=\"avg\">" << money(average) << "</dd></div>\n"
         << "    <div><dt>Largest account</dt><dd id=\"top\">" << escapeHtml(topName) << "</dd></div>\n"
         << "    <div><dt>Transactions</dt><dd id=\"tx\">" << transactions << "</dd></div>\n"
         << "  </dl>\n</div></header>\n"
         << "<main class=\"wrap\">\n"
         << "  <h2 class=\"title\">Accounts</h2>\n"
         << "  <p class=\"lede\">Deposit or withdraw on any card, or add an account below. The status, total and average update right away.</p>\n"
         << "  <section class=\"cards\" aria-label=\"Bank accounts\">\n";

    for (size_t i = 0; i < accounts.size(); i++) {
        file << accounts[i].toHtml();
    }

    file << "  </section>\n"
         << "  <form class=\"add\" id=\"add\" novalidate>\n"
         << "    <h3>Add an account</h3>\n"
         << "    <div class=\"fields\">\n"
         << "      <input id=\"newName\" type=\"text\" placeholder=\"Customer name\" aria-label=\"Customer name\" maxlength=\"40\">\n"
         << "      <input id=\"newBalance\" type=\"number\" min=\"0\" step=\"0.01\" inputmode=\"decimal\" placeholder=\"Starting balance\" aria-label=\"Starting balance\">\n"
         << "      <button type=\"submit\">Add account</button>\n"
         << "    </div>\n"
         << "    <p class=\"err\" id=\"addErr\" role=\"alert\"></p>\n"
         << "  </form>\n"
         << "  <p class=\"full\" id=\"full\" hidden>Maximum of " << MAX_ACCOUNTS << " accounts reached.</p>\n"
         << "</main>\n";
    file << kScript;
    file.close();
}

double printAccounts(const vector<BankAccount>& accounts) {
    double total = 0;
    for (size_t i = 0; i < accounts.size(); i++) {
        cout << "\nAccount " << (i + 1) << "\n";
        accounts[i].show();
        total += accounts[i].getBalance();
    }
    cout << "\n-------------------------------------\n";
    cout << "  Total money:     " << money(total) << "\n";
    cout << "  Average balance: " << money(total / accounts.size()) << "\n";
    cout << "-------------------------------------\n";
    return total;
}

int main() {
    vector<BankAccount> accounts;
    bool running = true;

    cout << "=====================================\n";
    cout << "      BANK ACCOUNT SIMULATOR\n";
    cout << "=====================================\n";

    while (running) {
        cout << "\nAccounts: " << accounts.size() << " of " << MAX_ACCOUNTS << "\n";
        cout << "1. Add an account\n";
        cout << "2. Make a deposit\n";
        cout << "3. View accounts\n";
        cout << "4. Finish and create website\n";
        int choice = (int)readNumber("Choose 1-4: ");

        if (choice == 1) {
            if ((int)accounts.size() >= MAX_ACCOUNTS) {
                cout << "Maximum of " << MAX_ACCOUNTS << " accounts reached.\n";
            } else {
                string name = "";
                while (name == "") {
                    cout << "  Name: ";
                    getline(cin, name);
                }
                double start = readNumber("  Starting balance: $");
                while (start < 0) {
                    cout << "  Starting balance cannot be negative.\n";
                    start = readNumber("  Starting balance: $");
                }
                accounts.push_back(BankAccount(name, start));
                cout << "  Account added for " << name << ".\n";
            }
        } else if (choice == 2) {
            if (accounts.empty()) {
                cout << "Add an account first.\n";
            } else {
                for (size_t i = 0; i < accounts.size(); i++) {
                    cout << "  " << (i + 1) << ". " << accounts[i].getName() << "\n";
                }
                int pick = (int)readNumber("Which account number: ");
                if (pick < 1 || pick > (int)accounts.size()) {
                    cout << "  No account with that number.\n";
                } else {
                    double amount = readNumber("Deposit amount: $");
                    if (accounts[pick - 1].deposit(amount)) {
                        cout << "  Deposit added. New balance: " << money(accounts[pick - 1].getBalance()) << "\n";
                    } else {
                        cout << "  Skipped: a deposit must be more than $0.\n";
                    }
                }
            }
        } else if (choice == 3) {
            if (accounts.empty()) {
                cout << "No accounts yet.\n";
            } else {
                printAccounts(accounts);
            }
        } else if (choice == 4) {
            if (accounts.empty()) {
                cout << "Add at least one account first.\n";
            } else {
                running = false;
            }
        } else {
            cout << "Please choose 1, 2, 3, or 4.\n";
        }
    }

    cout << "\n=====================================\n";
    cout << "           ACCOUNT SUMMARY\n";
    cout << "=====================================";
    double total = printAccounts(accounts);
    double average = total / accounts.size();

    makeWebsite(accounts, total, average);
    cout << "\nWebsite created: index.html (open it in your browser)\n";
    return 0;
}
