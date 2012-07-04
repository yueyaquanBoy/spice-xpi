/* ***** BEGIN LICENSE BLOCK *****
*   Copyright (C) 2012, Peter Hatina <phatina@redhat.com>
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License as
*   published by the Free Software Foundation; either version 2 of
*   the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
* ***** END LICENSE BLOCK ***** */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cctype>
#include "generator.h"

std::set<std::string> Generator::s_default_attributes;
std::set<std::string> Generator::s_default_methods;
std::map<std::string, std::string> Generator::s_default_attribute_values;

Generator::Generator(const std::list<Attribute> &attributes,
    const std::list<Method> &methods):
    m_attributes(attributes),
    m_methods(methods)
{
    init();
}

Generator::~Generator()
{
}

void Generator::init()
{
    if (!s_default_attributes.empty())
        return;

    s_default_attributes.insert("hostip");
    s_default_attributes.insert("port");
    s_default_attributes.insert("adminconsole");
    s_default_attributes.insert("hotkey");
    s_default_attributes.insert("smartcard");
    s_default_methods.insert("setlanguagestringssection");
    s_default_methods.insert("setlanguagestringslang");
    s_default_methods.insert("setusbfilterfilter");
    s_default_attribute_values["adminconsole"] = " checked";
    s_default_attribute_values["hotkey"] = " value=\"toggle-fullscreen=shift+f11," \
        "release-cursor=shift+f12,smartcard-insert=shift+f8,smartcard-remove=shift+f9\"";
    s_default_attribute_values["usblistenport"] = " value=\"32023\"";
}

void Generator::generate()
{
    generateHeader();
    generateConnectVars();
    generateContent();
    generateFooter();
}

void Generator::generateHeader()
{
    std::cout << "<html>\n"
              << "<head>\n"
              << "<title>Spice-XPI test page (generated)</title>\n"
              << "<style type=\"text/css\">\n"
              << "caption {\n"
              << "    text-align: left;\n"
              << "    font-weight: bold;\n"
              << "}\n\n"
              << "th {\n"
              << "    text-align: left;\n"
              << "}\n"
              << "</style>\n"
              << "</head>\n\n"
              << "<body onload=\"bodyLoad()\" onunload=\"bodyUnload()\">\n\n"
              << "<center>\n"
              << "<h1>SPICE xpi test page (generated)</h1>\n"
              << "SPICE xpi test page. Disabled (greyed out) values are passed\n"
              << "to SPICE xpi as empty variables.\n</center>\n<br/>\n\n"
              << "<embed type=\"application/x-spice\" width=\"0\" height=\"0\" id=\"spice-xpi\"/><br/>\n\n"
              << "<script type=\"text/javascript\">\n\n"
              << "var embed = document.getElementById(\"spice-xpi\");\n\n"
              << "function bodyLoad()\n{\n    log(\"Body Load\");\n};\n\n"
              << "function bodyUnload()\n{\n    log(\"Body Unload\");\n}\n\n"
              << "function connect()\n{\n"
              << "    setConnectVars();\n"
              << "    setUsbFilter();\n"
              << "    embed.connect();\n"
              << "    log(\"Connect: host '\" + embed.hostIP + \"', port '\" + "
              << "embed.port\n        + \"', secure port '\" + embed.SecurePort + "
              << "\"', USB port '\" +\n        embed.UsbListenPort + \"'\");\n}\n\n"
              << "function disconnect()\n{\n"
              << "    embed.disconnect();\n"
              << "    log(\"Disconnect\");\n}\n\n"
              << "function OnDisconnected(msg)\n{\n    log(\"Disconnected, return code: \" + msg);\n}\n\n"
              << "function log(message)\n{\n"
              << "    var log = document.getElementById(\"log\");\n"
              << "    var ts = new Date().toString() + \": \";\n"
              << "    var newRow = document.createElement(\"tr\");\n"
              << "    var tsCell = document.createElement(\"td\");\n"
              << "    var msgCell = document.createElement(\"td\");\n\n"
              << "    tsCell.innerHTML = ts;\n"
              << "    msgCell.innerHTML = message;\n\n"
              << "    newRow.appendChild(tsCell);\n"
              << "    newRow.appendChild(msgCell);\n"
              << "    log.appendChild(newRow);\n}\n\n"
              << "function setLanguageStrings()\n{\n"
              << "    section = document.getElementById(\"SetLanguageStringssectionToggled\").checked ?\n"
              << "        document.getElementById(\"SetLanguageStringssection\").value : \"\";\n"
              << "    lang = document.getElementById(\"SetLanguageStringslangToggled\").checked ?\n"
              << "        document.getElementById(\"SetLanguageStringslang\").value : \"\";\n"
              << "    embed.SetLanguageStrings(section, lang);\n"
              << "    log(\"Language Strings set to '\" + section + \"' '\" + lang + \"'\");\n}\n\n"
              << "function setUsbFilter()\n{\n"
              << "    UsbFilterToggled = document.getElementById(\"SetUsbFilterfilterToggled\");\n"
              << "    if (!UsbFilterToggled)\n        return;\n"
              << "    filter = UsbFilterToggled.checked ?\n"
              << "        document.getElementById(\"SetUsbFilterfilter\").value : \"\";\n"
              << "    embed.SetUsbFilter(filter);\n"
              << "    log(\"USB Filter String set to: '\" + filter + \"'\");\n}\n\n"
              << "function show()\n{\n"
              << "    embed.show();\n"
              << "    log(\"Show\");\n}\n\n"
              << "function ConnectedStatus()\n{\n"
              << "    log(\"Connected status = \" + embed.ConnectedStatus());\n}\n\n"
              << "function toggle(checkboxID)\n{\n"
              << "    var checkbox = document.getElementById(checkboxID);\n"
              << "    var toggle = document.getElementById(arguments[1]);\n"
              << "    toggle.disabled = !checkbox.checked;\n}\n\n";
}

void Generator::generateFooter()
{
    std::cout << "<hr/>\n<table style=\"border: 1px; border-color: black;\">\n"
              << "<caption>log:</caption>\n"
              << "<thead><tr><th style=\"width: 22em;\">timestamp</th>"
              << "<th>message</th></tr></thead>\n"
              << "<tbody style=\"font-family: monospace;\" id=\"log\">\n"
              << "</tbody>\n"
              << "</table>\n"
              << "</body>\n"
              << "</html>\n";
}

void Generator::generateConnectVars()
{
    std::cout << "function setConnectVars()\n{\n";
    std::list<Attribute>::iterator it;
    for (it = m_attributes.begin(); it != m_attributes.end(); ++it) {
        std::cout << "    embed." << it->getIdentifier() << " = "
                  << "document.getElementById(\""
                  << it->getIdentifier() << "Toggled\").checked ? "
                  << "document.getElementById(\""
                  << it->getIdentifier() << "\")."
                  << (it->getType() == Token::T_BOOLEAN ? "checked" : "value")
                  << " : \"\";\n";
    }
    std::cout << "}\n\n</script>\n\n";
}

void Generator::generateContent()
{
    std::cout << "<center>\n\n"
              << "<table id=\"values\">\n";

    std::list<Attribute>::iterator ita;
    for (ita = m_attributes.begin(); ita != m_attributes.end(); ++ita) {
        std::cout << "<tr>\n<td><input type=\"checkbox\" id=\""
                  << ita->getIdentifier() << "Toggled"
                  << "\" onclick=\"toggle('"
                  << ita->getIdentifier() << "Toggled"
                  <<"', '" << ita->getIdentifier()
                  << "')\" " << (attributeEnabled(*ita) ? "checked" : "")
                  << "/></td>\n"
                  << "<td>" << splitIdentifier(ita->getIdentifier()) << "</td>\n"
                  << "<td>" << attributeToHtmlElement(*ita)
                  << "</td>\n</tr>\n";
    }

    std::list<Method>::iterator itm;
    for (itm = m_methods.begin(); itm != m_methods.end(); ++itm) {
        std::list<Method::MethodParam>::iterator itp;
        std::list<Method::MethodParam> params = itm->getParams();
        for (itp = params.begin(); itp != params.end(); ++itp) {
            std::cout << "<tr>\n<td><input type=\"checkbox\" id=\""
                      << itm->getIdentifier() << itp->getIdentifier()
                      << "Toggled\" onclick=\"toggle('"
                      << itm->getIdentifier() << itp->getIdentifier()
                      << "Toggled', '" << itm->getIdentifier()
                      << itp->getIdentifier() << "')\""
                      << (methodEnabled(*itm, *itp) ? "checked" : "")
                      << "/></td>\n<td>" << splitIdentifier(itm->getIdentifier())
                      << " - " << splitIdentifier(itp->getIdentifier()) << "</td>\n"
                      << "<td><input id=\"" << itm->getIdentifier() << itp->getIdentifier()
                      << "\" type=\"" << (itp->getType() == Token::T_BOOLEAN ? "checkbox" : "text")
                      << "\" size=\"30\" " << (methodEnabled(*itm, *itp) ? "" : "disabled ")
                      << "/></td>\n</tr>\n";
        }
    }

    std::cout << "</table>\n\n<br/>\n";

    int i = 1;
    for (itm = m_methods.begin(); itm != m_methods.end(); ++itm, ++i) {
        std::cout << "<input type=\"button\" value=\""
                  << splitIdentifier(itm->getIdentifier())
                  << "\" style=\"min-width: 180px\" onclick=\""
                  << itm->getIdentifier()
                  << "()\"/>\n";
        if (i % 3 == 0 && i != static_cast<int>(m_methods.size()))
            std::cout << "<br/>\n";
    }

    std::cout << "\n</center>\n\n";
}

std::string Generator::lowerString(const std::string &str)
{
    std::string s(str);
    std::transform(s.begin(), s.end(), s.begin(), tolower);
    return s;
}

std::string Generator::splitIdentifier(const std::string &str)
{
    std::string result(str);
    result[0] = toupper(result[0]);
    for (size_t i = 1; i < result.size() - 1; ++i) {
        if (isupper(result[i]) && islower(result[i + 1]))
            result.insert(i++, " ");
        else if (isupper(result[i]) && islower(result[i - 1]))
            result.insert(i++, " ");
    }
    return result;
}

std::string Generator::attributeToHtmlElement(const Attribute &attr)
{
    std::stringstream ss;
    std::string id = lowerString(attr.getIdentifier());
    if (id == "truststore") {
        ss << "<textarea id=\"" << attr.getIdentifier()
           << "\" cols=\"66\" rows=\"33\" "
           << (attributeEnabled(attr) ? "" : "disabled")
           << "/></textarea>";
    } else {
        ss << "<input id=\"" << attr.getIdentifier() << "\" type=\""
           << (attr.getType() == Token::T_BOOLEAN ? "checkbox" : "text")
           <<"\" size=\"30\" " << attributeDefaultValue(attr)
          << (attributeEnabled(attr) ? "" : "disabled ") << "/>";
    }
    return ss.str();
}

std::string Generator::attributeDefaultValue(const Attribute &attr)
{
    std::string id(lowerString(attr.getIdentifier()));
    std::map<std::string, std::string>::iterator found = s_default_attribute_values.find(id);
    return found != s_default_attribute_values.end() ? found->second : "";
}

bool Generator::attributeEnabled(const Attribute &attr)
{
    std::string id(lowerString(attr.getIdentifier()));
    std::set<std::string>::iterator found = s_default_attributes.find(id);
    return found != s_default_attributes.end();
}

bool Generator::methodEnabled(const Method &method, const Method::MethodParam &param)
{
    std::string id(lowerString(method.getIdentifier() + param.getIdentifier()));
    std::set<std::string>::iterator found = s_default_methods.find(id);
    return found != s_default_methods.end();
}