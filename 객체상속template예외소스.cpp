/*
* Ŭ���� ���, template, ����ó�� ����
*/
#include <iostream>
#include <ctime>
using namespace std;
class ExceptionError {
public:
	virtual void Print() = 0;
};
class OverflowException : public ExceptionError {
public:
	void Print() {
		cout << "overflow ���� �߻�" << endl;
	}
};
class RowRecordsOverflowException : public OverflowException {
public:
	void Print() {
		cout << "Row Records overflow ���� �߻�" << endl;
	}
};
class TableRecordsOverflowException : public OverflowException {
public:
	void Print() {
		cout << "Table Records overflow ���� �߻�" << endl;
	}
};
class EmptyException : public ExceptionError {
public:
	void Print() {
		cout << "empty ���� �߻�" << endl;
	}
};
class NotFoundRecordException : public EmptyException {
public:
	void Print() {
		cout << "Not found record ���� �߻�" << endl;
	}
};
class Person {
protected:
	string pid;
	string pname;
public:
	string GetName() { return pname; }
	void SetName(char* str) { pname = str; }
};

class Employee : public Person {
private:
	string eno;
	string role;
public:
	Employee() :Person() {}
	Employee(string pid, string pname, string eno, string role) : Person(pid, pname), eno(eno), role(role) { }

	virtual void Print();
	virtual bool Equals(Employee* p);
};
bool Employee::Equals(Employee* p) {
	return pid == p->pid && pname == p->pname && eno == p->eno && role == p->role;
}
void Employee::Print() {
	cout << pid << ", " << pname << ", " << eno << ", " << role;
}


class Student {
private:
	string sid;
	string major;
public:
	void Print();
};
void Student::Print() {
	cout << sid << ", " << major << ", ";
}

class WorkStudent : public Student {
private:
	string job;
public:
	void Print();
};

void WorkStudent::Print() {
	Student::Print();
	cout << job;
}

// ���� ��, RowRecord�� function�� ������ ���̴�.
template <class T>
class RowRecords {
public:
	RowRecords(int maxKeys = 5, int unique = 1);// unique = 1�� �ߺ��� ����
	~RowRecords();
	void Clear(); // RowRecords�� ��� ���ڵ带 �����Ѵ�
	int Insert(const string key, T& record);
	int Remove(const string key, const T&);
	T& Search(const string key) const;
	void Print(ostream&) const;
	int numKeys()const { return NumKeys; }
	int Find(const string key) const;
protected:
	int MaxKeys;
	int NumKeys;
	string* Keys;
	T* Records;

	int Init(const int maxKeys, const int unique);
	int Unique; // if true, each key value must be unique

};

template <class T>
RowRecords<T>::RowRecords(int maxKeys, int unique) : NumKeys(0), Keys(0), Records(0) {
	Init(unique);
}
template <class T>
int RowRecords<T>::Init(const int maxKeys, const int unique) {
	Unique = unique != 0;
	MaxKeys = maxKeys;
	Keys = new String[maxKeys];
	Records = new T[maxKeys];
}
template <class T>
RowRecords<T>::~RowRecords() {
	delete Keys;
	delete Records;
}
template <class T>
void RowRecords<T>::Clear() {
	NumKeys = 0;
}
template <class T>
int RowRecords<T>::Insert(const string key, T& record) {
	int i;
	int index = Find(key);
	if (Unique && index >= 0) return 0; //�־��� key�� �̹� ������
	if (NumKeys == MaxKeys) throw OverflowInsert();
	for (i = NumKeys - 1; i >= 0; i--) {
		if (key > Keys[i])break; //insert into location i+1
		// insert�� �߰��� ���� ���̴�. ��, a, b, c, d, e �߿� b�� �ְ� �ʹٸ� c, d, e�� �� ĭ�� �ڷ� �о���� �Ѵ�.
		// insert�� ������ key������ �� ���� �ִ�.
		// 11, 22, 44, 55, 77 ���� 33�� �������� 77�� ���� ������ �з��� �Ѵ�. -> ������ point!
		// Key������ �����϶� �ϸ� �������. -> �ڷᱸ��, ���ϱ��� => ���迡 �ȳ�.
		// �迭�� ������ �����Ϸ��� �� �о���Ѵ�. -> ȿ��x => ���迡 �ȳ�.
		Keys[i + 1] = Keys[i];
		T[i + 1] = T[i];
	}
	Keys[i + 1] = key;
	T[i + 1] = record;
	NumKeys++;
	return 1;
}

// Record ���� �� ��, delete ����ؾ� �Ѵ�. ��ü�� main���� new�� �����Ա� ������ delete�� ��������� �Ѵ�. -> runtime-error �߻� ����.
template <class T>
int RowRecords<T>::Remove(const string key, const T& record) {
	int index = Find(key, record);
	if (index < 0) return 0; //key not in index
	for (int i = index; i < NumKeys; i++) {
		Keys[i] = Keys[i + 1];
		T[i] = T[i + 1];
	}
	NumKeys--;
	return 1;
}

template <class T>
T& RowRecords<T>::Search(const string key) const {
	int index = Find(key);
	if (index != -1) return Records[index];
	throw EmptyException();
}
template <class T>
void RowRecords<T>::Print(ostream& stream) const {
	stream << "max keys = " << MaxKeys << " num keys = " << NumKeys << endl;
	for (int i = 0; i < NumKeys; i++) {
		stream << "\tKey[" << i << "] " << Keys[i] << " Records = " << Records[i] << endl;
	}
}

// find()�� ���� �ٿ��� ���ϴ� key�� �ִ��� ã�� ��
// ������ key�� ������ �Է� ���Ѵ�.
// key�� ������ �Է��Ѵ�.
// ���� �ٿ��� key�� �ִ��� ã�� ���� �ؿ� ���� Find()
// 33�� �������� �ϴµ� ���� ù��° �ٿ� 33�� �ִٸ� �Է� ���ϰڴ�.
// -> �ߺ�üũ ������ ���迡 ���� �� �ִ�. (Key���� ������ �ȳְ�, Key���� ������ �ְڴ�.)

template <class T>
int RowRecords<T>::Find(const string key) const {
	for (int i = 0; i < NumKeys; i++) {
		if (Keys[i] < key) continue;
		if (Keys[i] == key) {
			if (record == Records[i])
				return i;
		}
	}
	return -1;
}

template <class T>
class RecordTable {
public:
	T& Read(string  key);//read by key
	int Append(const T& record);
	int Update(string key, const T& record);
	int Delete(string key);
	void Print(ostream&) const;
	RecordTable(int keySize, int maxKeys = 5, int maxRecords = 10);
	~RecordTable();
protected:
	int MaxKeys;
	int MaxRecords;
	int NumRecords;
	RowRecords<T>* data;
};
template <class T>
RecordTable<T>::RecordTable(int keySize, int maxKeys = 5, int maxRecords = 10) : MaxKeys(maxKeys), MaxRecords(maxRecords) {
	NumRecords = 0;
	for (int i = 0; i < MaxRecords; i++) {
		data[i] = new RowRecords<T>(maxKeys);
	}

}
template <class T>
RecordTable<T>::~RecordTable() {
	for (int i = 0; i < MaxRecords; i++) {
		delete data[i];
	}
}

// Read() : Key���� �־����� Record�� ã�ڴ�.
// Find(), Search()�� �̿��Ͽ� Read() ����.
// RowRecords�� function���� ����ؼ� ����.
// NotFoundRecord()�� ������?, ���ܰ�ü�� ������?
template <class T>
T& RecordTable<T>::Read(string key) {
	for (int i = 0; i < NumRecords; i++) {
		try {
			int result = data[i].Find(key);//data[i]�� row records�̰� ���⿡ key�� �ִ����� ����
		}
		catch () {

		}
		if (result > 0)
			return data[i].Search(key);//Search(key)�� ����Ͽ� row records���� �ش� ���ڵ带 �����´�

	}
	throw NotFoundRecord();
}
template <class T>
int RecordTable<T>::Append(const T& record) {
	string key = record.Key();
	int i = NumRecords;
	//NumRecords�� �ִ� row records�� append�� �� �ִ����� �����Ѵ�.
	if (data[i].numKeys() == MaxKeys)
	{
		// ���� ������ �ѱ�ڴ�.
		i = ++NumRecords;
	}
	// Append()�� Insert()�� ����.
	// ���� �߻� ���� : Insert() -> �� ���� ��á�� ��. table�� �� �� ���� �ƴ�.
	try {
		int result = data[i].Insert(record);
	}
	catch () {

	}

	return result;

}

// Record �ٲ�ġ�� -> Remove(), Insert() �̿�
template <class T>
int RecordTable<T>::Update(string key, const T& record) {
	for (int i = 0; i < NumRecords; i++) {
		int result = data[i].Find(key);//data[i]�� row records�̰� ���⿡ key�� �ִ����� ����
		if (result > 0) {
			data[i].Remove(key, Search(key));
			data[i].Insert(key, record);
			return 1;
		}


	}
	return 0;
}

// delete�� �ϴµ� �ش� key�� ������ �ִ� record�� ������ ���� �߻�.
// table�� �ƹ��͵� ���µ� �����Ϸ��� ���� �߻�
template <class T>
int RecordTable<T>::Delete(string key) {
}
template <class T>

// �� ó�� �����̺��� ���� ����� ���� ���� ������ ���� �߻�.
template <class T>
void RecordTable<T>::Print(ostream& os) const {
}

// RecordTable�� public �Լ���!
// �Է� -> Append()
// ��� -> Print()
// Read -> Read()
// ���� -> Delete()
// ���� ->  update()
// �������̶� �ٸ� �� : ã�µ� Key�� ����Ѵ�.

// Person(pid, pname) <= Employee(eno, role) : ���  ->  pid�� key�� ���� ���� �ְ�, eno�� key�� ���� ���� �ִ�.
// Student(sid, major) <= WorkStudent(job) : ���  ->  sid�� key�� ���� ���� �ְ�,  WorkStudent���� String wsid, String job �̷��� ������ ��, �� �� key�� �� �� �ִ�.

// Table�� �� ���� RowRecords��� class�� ����
// 10���̸� data[0] ~ data[9]
// 10���̸� MaxRecords = 10
// MaxKeys = ��
// MaxRecords = ��
// NumRecords = top : ���� �Է��ϰ� �ִ� ��
// 5���� �� ���� NumRecords++ �ؼ� ���� �ٷ� ���� ���̴�.

// RowRecords �� �ٿ���
// MaxKeys = 5 -> RowRecords �ȿ� ���� ���� �� �ִ� ������ ��
// NumKeys = �� -> �� ���� ���� �ٷ� ���� ��
// Keys = "hong"
// Records = "hong", "cse", "busan"
// Append()�� �ϰ� �Ǹ� Keys�� Records�� ���� �־����� �Ѵ�.
// ---------------------------------------------------------
// |	 Keys  | "hong" |			|		|		|		|
// ---------------------------------------------------------
// |	Records|		  |			|		|		|		|
// ---------------------------------------------------------

int main() {
	Employee* members[30];//Employee �������� �����ϴ� ���� �ذ� �ʿ� 
	WorkStudent* workers[20];
	RecordTable<Employee> etable(10, 5);//10���� record sets, �� set�� 5���� records
	RecordTable<WorkStudent> wtable(10, 5);
	int select;
	Employee* e; WorkStudent* ws;
	int result;
	while (1)
	{
		cout << "\n���� 1: member ��ü 30�� �Է�, 2: table ���, 3: table ��ü ã��, 4: table���� ��ü ����, 5: ����, 6: ����" << endl;
		cin >> select;
		switch (select) {
		case 1://table�� ��ü 30�� �Է�
			members[0] = new Employee("p1", "hong", "E1", "Coding");
			members[1] = new Employee("p2", "hee", "E2", "Coding");
			members[2] = new Employee("p3", "kim", "E3", "Test");
			members[3] = new Employee("p4", "lee", "E4", "Test");
			members[4] = new Employee("p5", "ko", "E5", "Design");
			members[5] = new Employee("p6", "choi", "E6", "Design");
			members[6] = new Employee("p7", "han", "E7", "Sales");
			members[7] = new Employee("p8", "na", "E8", "Sales");
			members[8] = new Employee("p9", "you", "E9", "Account");
			members[9] = new Employee("p10", "song", "E10", "Production");
			workers[0] = new WorkStudent("s011", "coding", "hong");
			workers[1] = new WorkStudent("s012", "coding", "ong");
			workers[2] = new WorkStudent("s013", "coding", "dong");
			workers[3] = new WorkStudent("s014", "coding", "fong");
			workers[4] = new WorkStudent("s015", "coding", "tong");
			workers[5] = new WorkStudent("s016", "coding", "nong");
			workers[6] = new WorkStudent("s017", "coding", "mong");
			workers[7] = new WorkStudent("s018", "coding", "kong");
			workers[8] = new WorkStudent("s019", "coding", "long");
			workers[9] = new WorkStudent("s020", "coding", "pong");
			workers[10] = new WorkStudent("s021", "coding", "lim");
			workers[11] = new WorkStudent("s022", "coding", "mim");
			workers[12] = new WorkStudent("s023", "coding", "bim");
			workers[13] = new WorkStudent("s024", "coding", "dim");
			workers[14] = new WorkStudent("s025", "coding", "rim");
			workers[15] = new WorkStudent("s026", "coding", "qim");
			workers[16] = new WorkStudent("s021", "coding", "fim");
			workers[17] = new WorkStudent("s021", "coding", "him");
			workers[18] = new WorkStudent("s027", "coding", "jim");
			workers[19] = new WorkStudent("s027", "coding", "jjj");

			try {
				for (int i = 0; i < 10; i++)
				{
					// tableoverflow �� ���⼭ catch
					// �� ���� overflow�Ǹ� main���� catch �� �ʿ� ����.
					// for loop���� try ~ catch �ϸ� �ʿ��� ��ġ�ϰ� continue -> �� ����.
					// �ٱ����� catch�ϸ� for loop �ߴ�
					// try {
					etable.Append(members[i]);
					//}
					//catch () {

					//}

				}
				for (int i = 0; i < 20; i++)
				{
					wtable.Append(workers[i]);

				}
			}

			catch () {

			}
			break;
		case 2:
			// table�� ��� ��ü ����ϱ�
			//table ������� ����ؾ� �Ѵ�: | *** | ???? | === |���� ����Ѵ�.

			etable.Print();
			wtable.Print();
			break;
		case 3:
			// table���� ��ü ã��
			try {
				e = etable.Read("kim");
				if (e == nullptr) cout << "kim�� �������� �ʴ´�" << endl;
				else
					e->Print();
			}
			catch () {

			}


			ws = wtable.Read("kim");
			if (ws == nullptr) cout << "kim�� �������� �ʴ´�" << endl;
			else
				ws->Print();
			break;
		case 4:
			//table���� ��ü �����ϱ�
			result = etable.Delete("hong");
			if (result > 0)
				cout << "������ records ����" << result << endl;
			result = wtable.Delete("hong");
			if (result > 0)
				cout << "������ records ����" << result << endl;
			break;
		case 5:
			result = etable.Update("hong", *e);
			if (result > 0)
				cout << "������ records ����" << result << endl;
			result = wtable.Update("hong", *ws);
			if (result > 0)
				cout << "������ records ����" << result << endl;
			break;

			// main���� try ~ catch�ϴ� ���
			//try {
			//	result = etable.Update("hong", *e);
			//	if (result > 0)
			//		cout << "������ records ����" << result << endl;
			//	result = wtable.Update("hong", *ws);
			//}
			//catch () {

			//}

			//if (result > 0)
			//	cout << "������ records ����" << result << endl;
			//break;
		default:
			exit(0);

		}
	}
	system("pause");
	return 1;
}