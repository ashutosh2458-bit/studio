// Design Studio core (C++17) - compiled to WebAssembly with Emscripten.
// Generates website HTML/CSS from design settings and builds GitHub URLs.
// The browser UI (index.html) only collects settings and displays results.

#include <cctype>
#include <string>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

namespace {

std::string g_full, g_css, g_linked, g_url;

enum Flags { NAV = 1, FEATURES = 2, QUOTE = 4, CTA = 8, FOOT = 16 };

struct Feature { std::string title, desc; };

std::string esc(const std::string &s) {
    std::string o;
    for (char c : s) {
        switch (c) {
            case '&': o += "&amp;"; break;
            case '<': o += "&lt;"; break;
            case '>': o += "&gt;"; break;
            case '"': o += "&quot;"; break;
            default: o += c;
        }
    }
    return o;
}

std::string pct(const std::string &s) {
    static const char *hex = "0123456789ABCDEF";
    std::string o;
    for (unsigned char c : s) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') o += char(c);
        else { o += '%'; o += hex[c >> 4]; o += hex[c & 15]; }
    }
    return o;
}

std::string trim(const std::string &s) {
    size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
    return a == std::string::npos ? "" : s.substr(a, b - a + 1);
}

// Only accept #rgb or #rrggbb so user input can never break out of the CSS.
std::string safeColor(const std::string &c) {
    if ((c.size() == 4 || c.size() == 7) && c[0] == '#') {
        for (size_t i = 1; i < c.size(); ++i)
            if (!std::isxdigit(static_cast<unsigned char>(c[i]))) return "#4f46e5";
        return c;
    }
    return "#4f46e5";
}

std::vector<Feature> parseFeatures(const std::string &text) {
    std::vector<Feature> out;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t end = text.find('\n', pos);
        if (end == std::string::npos) end = text.size();
        std::string line = text.substr(pos, end - pos);
        pos = end + 1;
        if (trim(line).empty()) continue;
        size_t bar = line.find('|');
        out.push_back({trim(line.substr(0, bar)),
                       bar == std::string::npos ? "" : trim(line.substr(bar + 1))});
    }
    return out;
}

std::string page(const std::string &title, const std::string &styleBlock, const std::string &body) {
    return "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\">\n"
           "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<title>" +
           esc(title) + "</title>\n" + styleBlock + "</head>\n<body>\n" + body + "</body>\n</html>\n";
}

} // namespace

extern "C" {

// Builds the design. Read results with get_full / get_css / get_linked.
EMSCRIPTEN_KEEPALIVE
void build_design(const char *layout_, const char *brand_, const char *headline_, const char *sub_,
                  const char *btn_, const char *feat_, const char *acc_, const char *font_,
                  int radius, int dark, int flags, int year) {
    const std::string layout = layout_, brand = brand_, headline = headline_, sub = sub_,
                      btn = btn_, font_key = font_;
    const std::string acc = safeColor(acc_);
    const std::vector<Feature> feats = parseFeatures(feat_);

    const std::string font =
        font_key == "serif" ? "Georgia,'Times New Roman',serif"
        : font_key == "mono" ? "ui-monospace,Menlo,Consolas,monospace"
                             : "system-ui,-apple-system,Segoe UI,sans-serif";
    const std::string bg = dark ? "#0f1115" : "#ffffff", fg = dark ? "#f2f3f5" : "#16181d",
                      mu = dark ? "#a0a6b2" : "#5b6270", cd = dark ? "#181b22" : "#f4f5f7",
                      ln = dark ? "#2a2e37" : "#e4e6ea";
    const int r = radius == 999 ? 8 : radius;
    const std::string br = std::to_string(r), br2 = std::to_string(r * 2), bt = std::to_string(radius);

    g_css =
        std::string("*{box-sizing:border-box}\n") +
        "body{margin:0;font-family:" + font + ";background:" + bg + ";color:" + fg + ";line-height:1.6}\n" +
        "a{color:inherit}\n.w{max-width:1000px;margin:0 auto;padding:0 24px}\n" +
        "nav{border-bottom:1px solid " + ln + "}\n" +
        "nav .w{display:flex;align-items:center;justify-content:space-between;height:60px}\n" +
        "nav b{font-size:18px}\nnav a{margin-left:20px;text-decoration:none;color:" + mu + ";font-size:14px}\n" +
        ".btn{display:inline-block;background:" + acc + ";color:#fff;padding:12px 26px;border-radius:" + bt +
        "px;text-decoration:none;font-weight:600}\n" +
        ".hero{padding:88px 0;text-align:center}\n" +
        ".hero h1{font-size:clamp(32px,6vw,56px);line-height:1.1;margin:0 0 16px}\n" +
        ".hero p{color:" + mu + ";font-size:18px;max-width:560px;margin:0 auto 28px}\n" +
        ".split{display:grid;grid-template-columns:1.1fr 1fr;gap:40px;align-items:center;text-align:left}\n" +
        ".split p{margin-left:0}\n" +
        ".vis{aspect-ratio:1;border-radius:" + br2 + "px;background:linear-gradient(135deg," + acc + ",#ec4899)}\n" +
        ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:18px;padding:24px 0 64px}\n" +
        ".card{background:" + cd + ";padding:24px;border-radius:" + br + "px}\n" +
        ".card h3{margin:0 0 6px}\n.card p{margin:0;color:" + mu + "}\n" +
        ".tile{aspect-ratio:4/3;border-radius:" + br + "px;display:flex;align-items:flex-end;padding:14px;color:#fff;font-weight:600}\n" +
        "blockquote{margin:0;padding:56px 0;text-align:center;font-size:22px;border-top:1px solid " + ln + "}\n" +
        "blockquote small{display:block;margin-top:10px;font-size:14px;color:" + mu + "}\n" +
        ".cta{background:" + acc + ";color:#fff;text-align:center;padding:64px 24px;border-radius:" + br2 +
        "px;margin:24px 0 64px}\n" +
        ".cta h2{margin:0 0 16px;font-size:30px}\n.cta .btn{background:#fff;color:" + acc + "}\n" +
        "footer{border-top:1px solid " + ln + ";padding:28px 0;color:" + mu + ";font-size:14px;text-align:center}\n" +
        "@media(max-width:700px){.split{grid-template-columns:1fr}.hero{padding:56px 0}}\n";

    std::string h;
    if (flags & NAV)
        h += "<nav><div class=\"w\"><b>" + esc(brand) +
             "</b><div><a href=\"#features\">Features</a><a href=\"#contact\">Contact</a></div></div></nav>\n";

    const std::string heroBody = "<h1>" + esc(headline) + "</h1>\n<p>" + esc(sub) +
                                 "</p>\n<a class=\"btn\" href=\"#contact\">" + esc(btn) + "</a>";
    if (layout == "split")
        h += "<header class=\"hero\"><div class=\"w split\"><div>" + heroBody +
             "</div><div class=\"vis\"></div></div></header>\n";
    else
        h += "<header class=\"hero\"><div class=\"w\">" + heroBody + "</div></header>\n";

    if (flags & FEATURES) {
        h += "<section class=\"w\" id=\"features\"><div class=\"grid\">";
        if (layout == "portfolio") {
            const char *cols[6] = {acc.c_str(), "#ec4899", "#0ea5e9", "#f59e0b", "#10b981", "#8b5cf6"};
            for (int i = 0; i < 6; ++i) {
                const std::string t = feats.empty() ? "Project" : feats[i % feats.size()].title;
                h += std::string("<div class=\"tile\" style=\"background:") + cols[i] + "\">" + esc(t) + "</div>";
            }
        } else {
            for (const Feature &f : feats)
                h += "<div class=\"card\"><h3>" + esc(f.title) + "</h3><p>" + esc(f.desc) + "</p></div>";
        }
        h += "</div></section>\n";
    }
    if (flags & QUOTE)
        h += "<blockquote class=\"w\">\xE2\x80\x9CWorking with " + esc(brand) +
             " changed how we think about growth.\xE2\x80\x9D<small>Happy client</small></blockquote>\n";
    if (flags & CTA)
        h += "<section class=\"w\" id=\"contact\"><div class=\"cta\"><h2>Ready to get started?</h2>"
             "<a class=\"btn\" href=\"mailto:hello@example.com\">" + esc(btn) + "</a></div></section>\n";
    if (flags & FOOT)
        h += "<footer><div class=\"w\">\xC2\xA9 " + std::to_string(year) + " " + esc(brand) +
             ". All rights reserved.</div></footer>\n";

    g_full = page(brand, "<style>\n" + g_css + "</style>\n", h);
    g_linked = page(brand, "<link rel=\"stylesheet\" href=\"style.css\">\n", h);
}

EMSCRIPTEN_KEEPALIVE const char *get_full()   { return g_full.c_str(); }
EMSCRIPTEN_KEEPALIVE const char *get_css()    { return g_css.c_str(); }
EMSCRIPTEN_KEEPALIVE const char *get_linked() { return g_linked.c_str(); }

// GitHub helpers: these open GitHub pages; no tokens or passwords are involved.
EMSCRIPTEN_KEEPALIVE
const char *github_new_repo_url(const char *repo) {
    const std::string r = trim(repo);
    g_url = "https://github.com/new" + (r.empty() ? std::string() : "?name=" + pct(r));
    return g_url.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char *github_file_url(const char *owner, const char *repo, const char *filename, const char *content) {
    g_url = "https://github.com/" + pct(trim(owner)) + "/" + pct(trim(repo)) + "/new/main?filename=" +
            pct(filename) + "&value=" + pct(content);
    return g_url.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char *github_invite_url(const char *owner, const char *repo) {
    g_url = "https://github.com/" + pct(trim(owner)) + "/" + pct(trim(repo)) + "/settings/access";
    return g_url.c_str();
}

} // extern "C"
