#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include "sha256.hpp"

using namespace std;

struct FileInfo
{
    string name;
    uint8_t day, month, year;
    uint32_t fileSize;
    uint8_t type;
};

static const string websiteList = R"(<!DOCTYPE html>
<html lang="en">
    <head>
        <title>Zombie's Cloud</title>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
    </head>
    <style>
        @font-face {
            font-family: 'Montserrat';
            src: url('MONTSERR.TTF');
        }
        body {
            white-space: nowrap;
            font-family: 'Montserrat';
            user-select: none;
            background-image: url('BG.PNG');
            background-position: center;
            background-size: 100px;
            image-rendering: pixelated;
            margin: 0;
            padding: 0;
            color: white;
        }
        table.files {
            clear: both;
            position: absolute;
            left: 2vw;
            float: left;
            font-size: large;
            width: 68vw;
            text-align: center;
            border-spacing: 0;
        }
        table.downloads {
            border-left: 2px solid #2F805C;
            font-size: large;
            position: absolute;
            left: 70vw;
            width: 28vw;
            width: 28vw;
            text-align: center;
            border-spacing: 0;
        }
        .headerheight {
            height: 50px;
        }
        .item:hover {
            background-color: #26955C;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.2s;
            transition-timing-function: ease;
        }
        .item:active {
            background-color: #3DC982;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.1s;
            transition-timing-function: ease;
        }
        .item {

            margin: 0;
            padding: 0;
            background-color: #0000005F;
            height: 50px;

            transition-property: background-color, color;
            transition-duration: 0.5s;
            transition-timing-function: ease;
        }
        .downloaditem {
            height: 50px;
        }
        .download:hover {
            background-color: #26955C;
            color: black;
            
            transition-property: background-color, color;
            transition-duration: 0.2s;
            transition-timing-function: ease;
        }
        .download:active {
            background-color: #3DC982;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.1s;
            transition-timing-function: ease;
        }
        .download {
            background-color: #0000005F;

            transition-property: background-color, color;
            transition-duration: 0.5s;
            transition-timing-function: ease;
        }
        td {
            margin: 0;
            padding: 0;
            border-top: none;
            border-left: none;
            border-right: none;
            border-bottom: 2px solid #6DD798;
        }
        th {
            font-size: larger;
            border-bottom: 2px solid #6DD798;
        }
        th:nth-child(1) {
            width: 40%;
        }
        th:nth-child(2) {
            width: 15%;
        }
        th:nth-child(3) {
            width: 15%;
        }
        nav {
            z-index: 5;
            position: fixed;
            top: 0;
            width: 100%;
            height: 50px;
            min-height: 50px;
            overflow: auto;
            margin: 0;
            padding: 0;
            border-bottom: 2px solid #2F805C;
            border-top: 2px solid #2F805C;
        }
        .navbar {
            margin: 0;
            float: left;
            width: 50%;
            height: 100%;
            line-height: 50px;
            text-align: center;
            font-size: xx-large;
        }
        .navbar:hover {
            background-color: #26955C;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.2s;
            transition-timing-function: ease;
        }
        .navbar:active {
            background-color: #3DC982;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.1s;
            transition-timing-function: ease;
        }
        .navbar {
            background-color: black;

            transition-property: background-color, color;
            transition-duration: 0.5s;
            transition-timing-function: ease;
        }
        .current {
            background-color: #20362B;
        }
        .icon_folder::before {
            content: url("FOLDER.PNG");
            position: relative;
            left: -5px;
            top: 2px;
        }
        .icon_image::before {
            content: url("IMAGE.PNG");
            position: relative;
            left: -5px;
            top: 2px;
        }
        .icon_video::before {
            content: url("VIDEO.PNG");
            position: relative;
            left: -5px;
            top: 2px;
        }
        .icon_other::before {
            content: url("OTHER.PNG");
            position: relative;
            left: -5px;
            top: 2px;
        }
        footer {
            position: fixed;
            right: 0;
            bottom: 0;
            width: 100%;
            font-size: small;
            text-align: right;
            margin-right: 5px;
            margin-bottom: 5px;
        }
        a {
            color: white;
        }
        @media only screen and (max-width: 650px) {
            body {
                white-space: wrap;
            }
            table.files {
                font-size: smaller;
            }
            table.downloads {
                font-size: smaller;
            }
            .icon_folder::before {
                top: 5px;
            }
            .icon_image::before {
                top: 5px;
            }
            .icon_video::before {
                top: 5px;
            }
            .icon_other::before {
                top: 5px;
            }
            .icon_folder {
                padding-bottom: 8px;
            }
            .icon_image {
                padding-bottom: 8px;
            }
            .icon_video {
                padding-bottom: 8px;
            }
            .icon_other {
                padding-bottom: 8px;
            }
        }
    </style>
    <body style="background-color: black;">
        <nav>
            //NAV
        </nav>
        <br>
        <br>
        <br>
        <table class="files">
            <tr class="headerheight">
                <th>File name</th>
                <th>Date modified</th>
                <th>File size</th>
            </tr>
            //DATA
        </table>
        <table class="downloads">
            <tr class="headerheight">
                <th>Download</th>
            </tr>
            //DOWN
        </table>
        <footer>
            Made by <a href="http://zombieschannel.com">Zombieschannel</a>, v.1.1
        </footer>
    </body>
</html>
)";

void addEntriesList(string& page, const vector<FileInfo>& data, const string& currentPath)
{
    string entries = "";
    string download = "";
    string nav = "<div class=\"navbar current\">List view</div>\n"
            "<div class=\"navbar\" onclick=\"window.open('" + currentPath + "?tile=1', '_self').focus();\">Tile view</div>\n";

    for (auto& n : data)
    {
        entries.append("<tr class=\"item\" onclick=\"window.open(\'" + currentPath + "/" + n.name + "\', '_blank').focus();\">\n");
        switch (n.type)
        {
        case 0:
            entries.append("<td class=\"icon_folder\">" + n.name + "</td>\n");
            break;
        case 1:
            entries.append("<td class=\"icon_image\">" + n.name + "</td>\n");
            break;
        case 2:
            entries.append("<td class=\"icon_video\">" + n.name + "</td>\n");
            break;
        default:
            entries.append("<td class=\"icon_other\">" + n.name + "</td>\n");
            break;
        }
        entries.append("<td>" + to_string(n.day) + "/" + to_string(n.month + 1) + "/" + to_string(n.year + 1900) + "</td>\n");
        if (n.fileSize > 800000)
        {
            const string size = to_string(n.fileSize / 1000000.f);
            entries.append("<td>" + size.substr(0, size.find('.') + 3) + " MB</td>\n");
        }
        else if (n.fileSize > 800)
        {
            const string size = to_string(n.fileSize / 1000.f);
            entries.append("<td>" + size.substr(0, size.find('.') + 3) + " KB</td>\n");
        }
        else
        {
            entries.append("<td>" + to_string(n.fileSize) + " B</td>\n");
        }
        entries.append("</tr>\n");
        download.append("<tr class=\"downloaditem\">\n");
        download.append("<td class=\"download\" onclick=\"window.open(\'" + currentPath + "/" + n.name  + "?download=1\', '_blank').focus();\">Download</td>\n");
        download.append("</tr>\n");
    }
    page.insert(page.find("//DATA"), entries).erase(page.find("//DATA"), 6);
    page.insert(page.find("//DOWN"), download).erase(page.find("//DOWN"), 6);
    page.insert(page.find("//NAV"), nav).erase(page.find("//NAV"), 5);
}

static const string websiteTile = R"(<!DOCTYPE html>
<html lang="en">
    <head>
        <title>Zombie's Cloud</title>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
    </head>
    <style>
        @font-face {
            font-family: 'Montserrat';
            src: url('MONTSERR.TTF');
        }
        body {
            white-space: nowrap;
            font-family: 'Montserrat';
            user-select: none;
            background-image: url('BG.PNG');
            background-position: center;
            background-size: 100px;
            image-rendering: pixelated;
            margin: 0;
            padding: 0;
            color: white;
        }
        nav {
            z-index: 5;
            position: fixed;
            top: 0;
            width: 100%;
            height: 50px;
            min-height: 50px;
            overflow: auto;
            margin: 0;
            padding: 0;
            border-bottom: 2px solid #2F805C;
            border-top: 2px solid #2F805C;
        }
        .navbar {
            margin: 0;
            float: left;
            width: 50%;
            height: 100%;
            line-height: 50px;
            text-align: center;
            font-size: xx-large;
        }
        .navbar:hover {
            background-color: #26955C;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.2s;
            transition-timing-function: ease;
        }
        .navbar:active {
            background-color: #3DC982;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.1s;
            transition-timing-function: ease;
        }
        .navbar {
            background-color: black;

            transition-property: background-color, color;
            transition-duration: 0.5s;
            transition-timing-function: ease;
        }
        .current {
            background-color: #20362B;
        }
        img {
            width: 120px;
            height: 120px;
        }
        .file {
            border: 1px solid #3DC982;
            margin: 2px;
            margin-bottom: 10px;
            float: left;
            font-size: large;
            
            width: 19%;
            height: 180px;
            display: flex;
            flex-direction: column;
        }
        .thumbnail {
            float: left;
        }
        .name {
            margin: 0;
            padding: 0;
            height: 35px;
            text-align: center;
        }
        .date {
            margin: 0;
            padding: 0;
            height: 35px;
            text-align: center;
        }
        .size {
            margin: 0;
            padding: 0;
            height: 35px;
            text-align: center;
        }
        .info {
            display: grid;
            place-items: center;
        }
        .download:hover {
            background-color: #26955C;
            color: black;
            
            transition-property: background-color, color;
            transition-duration: 0.2s;
            transition-timing-function: ease;
        }
        .download:active {
            background-color: #3DC982;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.1s;
            transition-timing-function: ease;
        }
        .download {
            background-color: #0000005F;
            flex: 1 1 100%;

            clear: left;

            display: grid;
            place-items: center;

            border: 1px solid #3DC982;

            transition-property: background-color, color;
            transition-duration: 0.5s;
            transition-timing-function: ease;
        }
        .innertop:hover {
            background-color: #26955C;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.2s;
            transition-timing-function: ease;
        }
        .innertop:active {
            background-color: #3DC982;
            color: black;

            transition-property: background-color, color;
            transition-duration: 0.1s;
            transition-timing-function: ease;
        }
        .innertop {
            background-color: #0000005F;
            border: 1px solid #3DC982;
            padding: 5px;

            transition-property: background-color, color;
            transition-duration: 0.5s;
            transition-timing-function: ease;
        }
        .container {
            display: flex;
            flex-direction: row;
            flex-wrap: wrap;
            justify-content: space-evenly;
        }
        footer {
            position: fixed;
            right: 0;
            bottom: 0;
            width: 100%;
            font-size: small;
            text-align: right;
            margin-right: 5px;
            margin-bottom: 5px;
        }
        a {
            color: white;
        }
        @media only screen and (max-width: 1800px) {
            .file {
                font-size: medium;
                width: 24%;
                height: 150px;
            }
            img {
                width: 100px;
                height: 100px;
            }
            .name {
                height: 30px;
            }
            .data {
                height: 30px;
            }
            .size {
                height: 30px;
            }
        }
        @media only screen and (max-width: 1100px) {
            .file {
                font-size: small;
                width: 30%;
                height: 130px;
            }
            img {
                width: 80px;
                height: 80px;
            }
            .name {
                height: 15px;
            }
            .data {
                height: 15px;
            }
            .size {
                height: 15px;
            }
        }
        @media only screen and (max-width: 650px) {
            body {
                white-space: wrap;
            }
            .file {
                font-size: x-small;
                width: 47%;
                height: 110px;
            }
            img {
                width: 65px;
                height: 65px;
            }
            .name {
                height: 10px;
            }
            .data {
                height: 10px;
            }
            .size {
                height: 10px;
            }
        }
        
    </style>
    <body style="background-color: black;">
        <nav>
            //NAV
        </nav>
        <br>
        <br>
        <br>
        <div class="container">
            //DATA
        </div>
        <footer>
            Made by <a href="http://zombieschannel.com">Zombieschannel</a>, v.1.1
        </footer>
    </body>
</html>)";

void addEntriesTile(string& page, const vector<FileInfo>& data, const string& currentPath)
{
    static unordered_map<string, string> hashmap;
    string entries = "";
    string nav = "<div class=\"navbar\" onclick=\"window.open('" + currentPath + "', '_self').focus();\">List view</div>\n"
            "<div class=\"navbar current\">Tile view</div>\n";

    for (auto& n : data)
    {
        entries.append("<div class=\"file\">\n");
        entries.append("<div class=\"innertop\" onclick=\"window.open(\'" + currentPath + "/" + n.name + "\', '_blank').focus();\">\n");
        string ID = n.name + to_string(n.fileSize);
        if (!hashmap.contains(ID))
            hashmap[ID] = sha256(ID);
        switch (n.type)
        {
        case 0:
            entries.append("<img src=\"/folder100.png\" class=\"thumbnail\">\n");
            break;
        case 1:
            entries.append("<img src=\"/thumb/" + hashmap[ID].substr(0, 8) + "." + hashmap[ID].substr(8, 3) + "\" class=\"thumbnail\">\n");
            break;
        case 2:
            entries.append("<img src=\"/video100.png\" class=\"thumbnail\">\n");
            break;
        default:
            entries.append("<img src=\"/other100.png\" class=\"thumbnail\">\n");
            break;
        }
        entries.append("<table class=\"info\">\n");
        entries.append("<tr><td class=\"name\">" + n.name + "</td></tr>");
        entries.append("<tr><td class=\"date\">" + to_string(n.day) + "/" + to_string(n.month + 1) + "/" + to_string(n.year + 1900) + "</td></tr>\n");
        if (n.fileSize > 800000)
        {
            const string size = to_string(n.fileSize / 1000000.f);
            entries.append("<tr><td class=\"size\">" + size.substr(0, size.find('.') + 3) + " MB</td></tr>\n");
        }
        else if (n.fileSize > 800)
        {
            const string size = to_string(n.fileSize / 1000.f);
            entries.append("<tr><td class=\"size\">" + size.substr(0, size.find('.') + 3) + " KB</td></tr>\n");
        }
        else
        {
            entries.append("<tr><td class=\"size\">" + to_string(n.fileSize) + " B</td></tr>\n");
        }
        entries.append("</table>\n");
        entries.append("</div>\n");
        entries.append("<div class=\"download\" onclick=\"window.open(\'" + currentPath + "/" + n.name + "?download=1\', '_blank').focus();\">Download</div>\n");
        entries.append("</div>\n");
    }
    page.insert(page.find("//DATA"), entries).erase(page.find("//DATA"), 6);
    page.insert(page.find("//NAV"), nav).erase(page.find("//NAV"), 5);
}