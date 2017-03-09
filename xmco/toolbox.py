# -*- coding: utf-8 -*-

import urllib.request
from lxml import html
import sqlite3
from threading import Lock

non_desired_file_type = ["pdf", "jpg", "dng", "mov", "zip", "png"]
primary_key = 0
primary_key_lock = Lock()


def increment_key():
    """
    Safely Increment primary key by 1
    :return : primary
    """
    global primary_key
    primary_key_lock.acquire()
    primary_key += 1
    primary_key_lock.release()
    return primary_key


def get_urls_from_page(parent_url):
    """
    Retrieve all hyperlinks from given url (check for <a> element) and remove duplicates
    :param parent_url: url of the address to examine
    :return: a list of all url hyperlinks found
    """

    if type(parent_url) != str:
        raise TypeError
    elif len(parent_url) <= 0:
        raise ValueError

    hyperlink_list = list()

    try:
        page = html.fromstring(urllib.request.urlopen(parent_url).read())
    except urllib.error.HTTPError:
        return hyperlink_list
    except urllib.error.URLError:
        return hyperlink_list
    except ValueError:
        return hyperlink_list

    for xlink in page.xpath("//a"):
        link = xlink.get("href")
        if link is not None and len(link) > 0:
            if not is_absolute_link(link):
                link = parent_url + link[1:]
            hyperlink_list.append(link)

    hyperlink_list = [url for url in hyperlink_list if is_absolute_link(url) and
                      is_url_link_to_page(url) and
                      is_string_in_url("www.xmco.fr", url) and
                      not is_string_in_url("ailto", url)]

    return list(set(hyperlink_list))


def is_url_secure(url):
    """
    Check if given url is secure or not
    :param url: url to be checked
    :return: 1 if url secure, 0 if not
    """

    if type(url) != str:
        raise TypeError
    elif len(url) <= 0:
        raise ValueError

    if "https" in url:
        return 1
    else:
        return 0


def is_absolute_link(link):
    """
    Check if link is absolute or relative
    :param link: Link to be check
    :return: True if absolute link, False otherwise.
    """

    if type(link) != str:
        raise TypeError
    elif len(link) <= 0:
        raise ValueError

    return link.startswith("http")


def is_url_link_to_page(url):
    """
    Check if the url links to an other HTLM page, not a media file (pdf, jpg, mov...)
    :param url: url to be checked
    :return: True if url is a real link, False if it is a media file
    """

    if type(url) != str:
        raise TypeError
    elif len(url) <= 0:
        raise ValueError

    last_url_branch = url.split("/")[-1]

    if last_url_branch.split(".")[-1] in non_desired_file_type:
        return False

    return True


def is_string_in_url(string, url):
    """
    Check if a string is present in URL
    :param string: string to search for
    :param url: url to be parsed
    :return: True if full string is present in url, False otherwise
    """

    if type(string) != str or type(url) != str:
        raise TypeError
    elif len(string) <= 0 or len(string) <= 0:
        raise ValueError

    if string in url:
        return True

    return False


def construct_database_entry(depth, url):
    """
    Construct tuple for database entrance
    :param url: given url to enter in database
    :param depth: depth in witch is located the database
    :return: tuple with (id, secure, depth, url)
    """

    if type(url) != str or type(depth) != int:
        raise TypeError
    elif len(url) <= 0 or depth < 0:
        raise ValueError

    id = increment_key()

    return id, is_url_secure(url), depth, url


def create_database(path_to_database):
    """
    Function to create the database
    :param path_to_database: path the create the database
    :return: tuple composed of connection to db and cur to it
    """

    if type(path_to_database) != str:
        raise TypeError
    if len(path_to_database) <= 0:
        raise ValueError

    conn = sqlite3.connect(path_to_database)

    cur = conn.cursor()
    cur.execute("CREATE TABLE page_url ( \
                        id INTEGER PRIMARY KEY, \
                        secure INTEGER, \
                        depth INTEGER, \
                        url TEXT)"
                )

    return conn, cur


def pretty_print_database(entries_list):
    """
    Pretty print the database output
    :param entries_list: all the entries found on the database
    :return the string needed
    """

    if type(entries_list) != list or len(entries_list) <= 0:
        raise TypeError

    pretty = "| ID  | URL" + " "*85 + "| Depth | Secure |\r\n"
    pretty += "| --- |" + "-"*89 + "| ----- | ------ |\r\n"

    for entry in entries_list:
        pretty += "| {}".format(entry[0]) + " " * (4-len(str(entry[0]))) + "| {}".format(entry[3]) + \
                  " "*(88-len(entry[3])) + "| {}     | {}      |\r\n".format(entry[2], entry[1])
    return pretty


if __name__ == "__main__":
    pass
