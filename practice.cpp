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
	string pid;
	string pname;
public:
	Person() {}
	Person(string pid, string pname) :pid(pid), pname(pname) { }
	string GetName() { return pname; }
	void SetName(char* str) { pname = str; }
	string GetId() { return pid; }
	virtual ~Person() {}
};

class Employee : public Person {
private:
	string eno;
	string role;
public:
	Employee() :Person() {}
	Employee(string pid, string pname, string eno, string role) :Person(pid, pname), eno(eno), role(role) {}
	virtual void Print();
	virtual bool Equals(Employee* p);
	string Key() { return GetName(); }
};
bool Employee::Equals(Employee* p) {
	return (eno == p->eno && role == p->role &&
		this->GetName() == p->GetName() && this->GetId() == p->GetId());
}
void Employee::Print() {
	cout << " eno: " << eno << " role: " <<
		role << " pid: " << GetId() << " pname: " << GetName() << endl;
}


class Student {
private:
	string sid;
	string major;
public:
	virtual void Print();
	Student() {}
	Student(string sid, string major) : sid(sid), major(major) {}
	string getSid() { return sid; }
	string getMajor() { return major; }

};
void Student::Print() {

	cout << " sid: " << sid << " major: " << major;
}
class WorkStudent : public Student {
private:
	string job;
public:
	WorkStudent() { Student(); }
	WorkStudent(string sid, string major, string job) :Student(sid, major), job(job) {}
	bool Equals(WorkStudent* p);
	void Print();
	string Key() { return job; }
};

bool WorkStudent::Equals(WorkStudent* p) {
	return (getSid() == p->getSid() && getMajor() == p->getMajor() && job == p->job);
}

void WorkStudent::Print() {
	Student::Print();
	cout << " job: " << job << endl;
}

template <class T>
class RowRecords {//�� Ŭ������ �޼��忡�� �Ķ���ʹ� �� �� �ٵ� ���� �� ����
public:
	RowRecords(int maxKeys, int unique = 1);// unique = 1�� �ߺ��� ����
	~RowRecords();
	void Clear(); // RowRecords�� ��� ���ڵ带 �����Ѵ�//���� ���� ��
	//�Ʒ��� 6�� �޼��带 �̿��ؼ� ����,�̰� ���� ���ٰ���
	int Insert(const string key, T& record);//
	int Remove(const string key);
	T& Search(const string key) const;
	void Print(ostream&) const;
	int numKeys()const { return NumKeys; }
	int Find(const string key) const;
protected:
	int MaxKeys;//5��
	int NumKeys;//top ���� ���� �ְ� �ִ� �� �ְ� ����++
	string* Keys;//key�� MaxKeys�� �Ѱ� ������ ���� ���� ����
	T* Records;//Keys������ ���ƾ���
	int DeleteNumber;

	//int Init(int maxKeys, int unique);
	int Init(int unique);
	int Unique; // if true, each key value must be unique

};

template <class T>
RowRecords<T>::RowRecords(int maxKeys, int unique = 1) : NumKeys(0) {//maxKeys=10�� �Է¹���
	MaxKeys = maxKeys;//5
	Init(unique);//1
}
template <class T>
int RowRecords<T>::Init(int unique) {
	Unique = unique != 0;
	//MaxKeys = maxKeys;
	Keys = new string[MaxKeys];
	Records = new T[MaxKeys];
	DeleteNumber = 0;
	return 1;
}

template <class T>
RowRecords<T>::~RowRecords() {
	delete[] Keys;
	delete[] Records;
}

template <class T>
void RowRecords<T>::Clear() {
	NumKeys = 0;
}
template <class T>
int RowRecords<T>::Insert(const string key, T& record) {//append���� �ҷ������� �׳� ���� �ڿ� ���̱�
	int i;
	int index = Find(key);
	if (Unique && index >= 0) return 0; //�־��� key�� �̹� ������
	if (NumKeys == MaxKeys) throw RowRecordsOverflowException();
	for (i = NumKeys - 1; i >= 0; i--) {//Ű�� ������ �̸� ������ ����//���� �ٷ� �Ѿ ���� ����//Ű�� ���� ��������--->�̰��ϸ� 2/3�� ���� ������//
		if (key > Keys[i])break; //insert into location i+1
		Keys[i + 1] = Keys[i];
		Records[i + 1] = Records[i];
	}
	Keys[i + 1] = key;
	Records[i + 1] = record;
	NumKeys++;
	cout << "NumKeys: " << NumKeys << endl;
	return 1;
}

template <class T>
int RowRecords<T>::Remove(const string key) {//C++ ������ ���ڵ� ������ �� Delete ����� ��
	int index = Find(key);
	if (index < 0) return 0; //key not in index
	for (int i = index; i < NumKeys - 1; i++) {
		Keys[i] = Keys[i + 1];//Delete�� ���ϰ� ������ �����͸� �ű�⸸ �ϸ� runTimeError�� �����
		Records[i] = Records[i + 1];
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
		stream << "\tKey[" << i << "] " << Keys[i] << " Records = ";
		Records[i].Print();
	}
}

template <class T>
int RowRecords<T>::Find(const string key) const {//�̹� �� Ű�� ������ �Է����� �ʱ� ���ؼ� �ߺ��ȵǰ� �ϱ� ���� �޼���
	for (int i = 0; i < NumKeys; i++) {
		if (Keys[i] < key) continue;
		if (Keys[i] == key) {
			return i;
		}
	}
	return -1;
}

template <class T>
class RecordTable {
public:
	T& Read(string  key);//read by key
	int Append(T& record);
	int Update(string key, const T& record);
	int Delete(string key);
	void Print(ostream&) const;
	RecordTable(int maxKeys = 5, int maxRecords = 10);
	~RecordTable();
protected:
	int MaxKeys;//��
	int MaxRecords;//��
	int NumRecords;//���� �Է��ϰ� �ִ� �� top �� �� �� ä��� ++�ϱ�
	RowRecords<T>** data;//�� ���� RowRecords�� ���� 10���̸� data[0] ~ data[9]
};

template <class T>
RecordTable<T>::RecordTable(int maxKeys, int maxRecords) : MaxKeys(maxKeys), MaxRecords(maxRecords) {
	NumRecords = 0;
	data = new RowRecords<T> *[MaxKeys];
	for (int i = 0; i < MaxKeys; i++) {//5�� �ݺ�
		data[i] = new RowRecords<T>(MaxRecords);//10�� ����
	}

}
//template <class T>
//RecordTable<T>::~RecordTable() {
//   for (int i = 0; i < MaxRecords; i++) {
//      delete data[i];
//   }
//}

template <class T>
RecordTable<T>::~RecordTable() {
	delete[] data;
}

template <class T>
T& RecordTable<T>::Read(string key) {
	for (int i = 0; i < NumRecords; i++) {//���ٿ��� key�� ã������ Ž��
		try {
			int result = data[i]->Find(key);//data[i]�� row records�̰� ���⿡ key�� �ִ����� ����
			if (result > 0)
				return data[i]->Search(key);//Search(key)�� ����Ͽ� row records���� �ش� ���ڵ带 �����´�
		}

		catch (ExceptionError& error) {
			error.Print();
		}

	}
	throw NotFoundRecordException();//nullptr return�������� �̰ɷ� ����ó���ϱ�

}

template <class T>
int RecordTable<T>::Append(T& record) {//���� ������ �ϴ� ���� ���� ������+try,catch�� �ؾ���
	string key = record.Key();
	int i = NumRecords;
	if (NumRecords == MaxKeys) {
		throw TableRecordsOverflowException();
		return 0;
	}
	//NumRecords�� �ִ� row records�� append�� �� �ִ����� �����Ѵ�.
	if (data[i]->numKeys() == MaxRecords)//�� ���� �� á�ٴ� ����
	{
		i = ++NumRecords;
	}
	try {//���ܹ߻� ���ϸ� ���� ���� ����
		int result = data[i]->Insert(key, record);
		return result;
	}
	catch (ExceptionError& error) {

		error.Print();
		return 0;

	}

	//return result;

}
//template <class T>
//int RecordTable<T>::Update(string key, const T& record) {
//   for (int i = 0; i < NumRecords; i++) {
//      int result = data[i].Find(key);//data[i]�� row records�̰� ���⿡ key�� �ִ����� ����
//      if (result > 0) {
//         data[i].Remove(key, Search(key));
//         data[i].Insert(key, record);
//         return 1;
//      }
//
//
//   }
//   return 0;
//}
template <class T>
int RecordTable<T>::Delete(string key) {//�����Ϸ��µ� Ű�� ������ ���ܹ߻��� ���Ѿ� ��
	if (NumRecords == 0) {
		throw EmptyException();
		return 0;
	}

	for (int i = 0; i < NumRecords + 1; i++) {
		try {
			return data[i]->Remove(key);

		}
		catch (ExceptionError& error) {
			error.Print();
		}
	}


}

template <class T>
void RecordTable<T>::Print(ostream& os) const {//�� ������� �� ����Ϸ��� �ϸ� ���ܸ� �߻����Ѿ� ��
	if (NumRecords == 0) {
		throw EmptyException();
		return;
	}

	for (int i = 0; i < NumRecords + 1; i++) {
		try {
			data[i]->Print(os);
		}
		catch (ExceptionError& error) {
			error.Print();
		}
	}
}

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
		cout << "\n���� 1: member  ��ü 30�� �Է�(Append()), 2.table ���(Print()) , 3: table ��ü ã�� (Read()) 4: table���� ��ü ����(Delete()), 5: ����(update())" << endl;//()���� �� RecordTable�� public �Լ� �� 
		cin >> select;
		switch (select) {
		case 1://table�� ��ü 30�� �Է�
		{
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


			for (int i = 0; i < 10; i++)
			{
				try {
					etable.Append(*members[i]);//overflow���ܴ� ��µǾ���� table overflow�� main���� �ؾ�������, �� ���� overflow�� �޼��忡�� ��
				}
				catch (ExceptionError& error) {
					error.Print();
				}
			}

			cout << "2��°�� ���Ϳ�" << endl;


			for (int i = 0; i < 20; i++)
			{
				try {
					wtable.Append(*workers[i]);//
				}
				catch (ExceptionError& error) {
					error.Print();
				}
			}



			break;
		}
		case 2:
			// table�� ��� ��ü ����ϱ�
			//table ������� ����ؾ� �Ѵ�: | *** | ???? | === |���� ����Ѵ�.
			try {
				etable.Print(cout);
			}
			catch (ExceptionError& error) {
				error.Print();
			}
			try {
				wtable.Print(cout);
			}
			catch (ExceptionError& error) {
				error.Print();
			}
			break;
		case 3:
			// table���� ��ü ã��
			try {
				e = &etable.Read("kim");



			}
			catch (ExceptionError& error) {
				error.Print();
			}
			try {
				ws = &wtable.Read("kim");
			}
			catch (ExceptionError& error) {
				error.Print();
			}
			break;
		case 4:
			//table���� ��ü �����ϱ�
			try {
				result = etable.Delete("hong");
				if (result > 0)
					cout << "������ records ����" << result << endl;
			}
			catch (ExceptionError& error) {
				error.Print();
			}

			try {
				result = wtable.Delete("hong");
				if (result > 0)
					cout << "������ records ����" << result << endl;
			}
			catch (ExceptionError& error) {
				error.Print();
			}
			break;
			//case 5:
			//   try {

			//      result = etable.Update("hong", *e);
			//      if (result > 0)
			//         cout << "������ records ����" << result << endl;
			//      result = wtable.Update("hong", *ws);
			//   }
			//   catch () {
			//   }

			//   if (result > 0)
			//      cout << "������ records ����" << result << endl;
			//   break;
		default:
			exit(0);

		}
	}

	system("pause");
	return 1;
}