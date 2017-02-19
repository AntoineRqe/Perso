# coding: utf8


class ZDict():
    """
    Class to get an ordered dictionary
    TP from OpenClassRoom
    """

    def __init__(self, given_dict={}, **kwargs):
        self.keys_list = []
        self.values_list = []

        if type(given_dict) not in (dict, ZDict):
            raise TypeError("Not a dict or a ZDict")

        elif (type(given_dict) == dict) & (len(given_dict) != 0):
            print("dict given {} with type {}".format(given_dict, type(given_dict)))
            for key, value in given_dict.items():
                self.keys_list.append(key)
                self.values_list.append(value)

        elif (type(kwargs) == dict) & (len(kwargs) != 0):
            print("params given {} with type {}".format(kwargs, type(kwargs)))
            for key, value in kwargs.items():
                self.keys_list.append(key)
                self.values_list.append(value)

    def __repr__(self):
        i = 0
        string = str("{")
        while i < len(self.keys_list):
            string += "'{}':{}, ".format(self.keys_list[i], self.values_list[i])
            i += 1
        string = string[:-2]
        string += "}"
        return string

    def __len__(self):
        return len(self.keys_list)

    def __getitem__(self, item):
        if item not in self.keys_list:
            raise KeyError("Key not in dict")
        return {self.keys_list[item]: self.values_list[item]}

    def __setitem__(self, key, value):
        for i, cle in enumerate(self.keys_list):
            if cle == key:
                self.values_list[i] = value
                return

        self.keys_list.append(key)
        self.values_list.append(value)

    def __delitem__(self, key):
        if key not in self.keys_list:
            raise KeyError("Key not in the list")
        for i, cle in enumerate(self.keys_list):
            if cle == key:
                del self.keys_list[i]
                del self.values_list[i]

    def __contains__(self, item):
        for cle in self.keys_list:
            if cle == item:
                return True
        return False

    def __iter__(self):
        return iter(self.keys_list)

    def sort(self):
        sorted_key = sorted(self.keys_list)
        sorted_list = list()
        i = 0
        while i < len(self.keys_list):
            for j,cle in enumerate(self.keys_list):
                if cle == sorted_key[i]:
                    sorted_list.append(self.values_list[j])
                    break
            i += 1
        self.keys_list.sort()
        self.values_list = list(sorted_list)

    def reverse(self):
        sorted_key = sorted(self.keys_list, reverse=True)
        sorted_list = list()
        i = 0
        while i < len(self.keys_list):
            for j,cle in enumerate(self.keys_list):
                if cle == sorted_key[i]:
                    sorted_list.append(self.values_list[j])
                    break
            i += 1
        self.keys_list.sort(reverse=True)
        self.values_list = list(sorted_list)

    def values(self):
        return self.values_list

    def keys(self):
        return self.keys_list

    def items(self):
        item_list = list()
        for i, key in enumerate(self.keys_list):
            item_list.append((key, self.values_list[i]))
        return item_list

    def __add__(self, other):
        for key in other.keys_list:
            self.keys_list.append(key)
        for value in other.values_list:
            self.values_list.append(value)


def main():
    dict_test = {'a':"a", 'c': "c", 'b': "b", 'd': "d"}
    dict_test2 = {'e': "e", 'f': "f", 'g': "g", 'h': "h"}
    my_dict = ZDict(dict_test)
    my_dict2 = ZDict(dict_test2)
    print "{} with len {}".format(my_dict, len(my_dict))
    my_dict['test0'] = "test0"
    my_dict['test1'] = "test1"
    my_dict['test2'] = "test2"
    print "{} with len {}".format(my_dict, len(my_dict))
    del(my_dict["test1"])
    print "{} with len {}".format(my_dict, len(my_dict))

    my_dict.sort()
    print "{} with len {}".format(my_dict, len(my_dict))
    my_dict.reverse()
    print "{} with len {}".format(my_dict, len(my_dict))

    for cle, value in my_dict.items():
        print "{} : {}".format(cle, value)

    print my_dict2

    my_dict + my_dict2
    print my_dict

    for i in range (0, len(my_dict)):
        print my_dict[i]

if __name__ == "__main__":
    main()