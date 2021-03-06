import json

class Database:
    def __init__(self, file):
        self.file = file
        try:
            with open(file,"r") as write_file:
                self.data = json.load(write_file)
                write_file.close()
        except:
            open(file,"w")
            self.data=[]


    def addItem(self,name,ID,x,y):
        name=name.lower()
        for i in self.data:
            if i["name"] == name:
                return False
            if i["row"] == y and i["col"] == x and i["shelf_id"]==ID:
                return False
        with open(self.file,"w") as write_file:
            self.data.append({"Name":name , "shelf_id":ID, "row":y, "col":x})
            self.data = sorted(self.data, key=lambda i: i["shelf_id"])
            json.dump(self.data,write_file)
        return True

    def search(self,Name,ID,start = 0,end = None):
        Name = Name.lower()
        for i in self.data:
            if i["shelf_id"]==ID and i["name"].lower()==Name:
                return i
        # half = len(self.data)//2
        # if end==None:
        #     end = len(self.data)
        # if ID==self.data[half]["Shelf_ID"]:
        #     for i in self.data[]
        # elif self.data[half]["Shelf_ID"]<ID:
        #     return self.search(Name,ID,start=start,end = half)
        # else:
        #     return self.search(Name,ID,start=half,end=end)
    def removeItem(self,Name,ID):
        item = self.search(Name,ID)
        self.data.remove(item)
        with open(self.file,"w") as write_file:
            json.dump(self.data,write_file)




if __name__ == "__main__":
    data = Database("./database.json")
    test="1"
    while test != "":
        test = input("1:Add 2:Remove 3:Search (Enter to exit):")
        if test == "1":
            datain = input("Input Data (Name,ID,Row,Column):").split(",")
            data.addItem(datain[0],datain[1],int(datain[2]),int(datain[3]))
        if test == "2":
            rem = input("Input Name,Shelf ID:").split(",")
            data.removeItem(rem[0],rem[1])
        if test == "3":
            search = input("Input Name,Shelf ID:").split(",")
            print(data.search(search[0],search[1]))
