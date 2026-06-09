#include<iostream>
#include<algorithm>
#include<math.h>
#include<unordered_map>
#include<queue>
#include<fstream>
#include<ctime>
#include<limits>
using namespace std;


int read_int(){
    int x;
    while(true){
        cin>>x;
        if(!cin.fail()){
            return x;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout<<"输入无效，请输入数字！"<<endl;
    }
}

int read_int_range(int l,int r){
    int x;
    while(true){
        x=read_int();
        if(x>=l&&x<=r){
            return x;
        }
        cout<<"输入范围错误，请输入"<<l<<"到"<<r<<"之间的数字！"<< endl;
    }
}

double read_double(){
    double x;
    while(true){
        cin>>x;
        if(!cin.fail()){
            return x;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout<<"输入无效，请输入数字！"<< endl;
    }
}


time_t publish_time[1010]; // 存储该项目成绩公示的时间


const int N=101010,MAX_DEPT=1001,MAX_PRO=1001;
//名称哈希 名称转编号
unordered_map<string,int>depart_idx;
unordered_map<string,int>pro_idx;

//提交的申诉
queue<int>propose;

//院系总分
unordered_map<string,double>grade_sum;

//编号转名称
string pid_to_name[N];
string did_to_name[N];

//每个项目前三
string pro_rank3[1010][10];

//各项目状态 0为未公示 1为待复核状态 2为不允许修改 3为公示中：
int state[1010];

//各院系 各项目排名：
int dep_p_r[1010][1001];

//院系
int dep_count;
int idx1;
struct depart{
    string name;
    int code;
}dep_list[N];


//项目
int idx2;
int pro_count;
struct project{
    int code;
    string name;
    string type;
    double score;
}pro_list[N];



void save_data(){
    ofstream fout("data.txt");

    if(!fout){
        cout<<"数据保存失败！"<<endl;
        return;
    }

    // 1. 保存院系数量
    fout<<dep_count<<endl;

    // 2. 保存院系信息：编号 名称 总分
    for(int i=1;i<=idx1;i++){
        fout<<dep_list[i].code<<" "
            <<dep_list[i].name<<" "
            <<grade_sum[dep_list[i].name]<<endl;
    }

    // 3. 保存项目数量
    fout<<pro_count<<endl;

    // 4. 保存项目信息：编号 名称 类型 分数 状态
    for(int i=1;i<=idx2;i++){
        int code=pro_list[i].code;
        string key=pro_list[i].name+pro_list[i].type;

        fout<<code<<" "
            <<pro_list[i].name<<" "
            <<pro_list[i].type<<" "
            <<pro_list[i].score<<" "
            <<state[code]<<" ";

        // 保存前三名
        for(int j=1;j<=3;j++){
            if(pro_rank3[code][j]==""){
                fout<<"NULL ";
            }
            else{
                fout<<pro_rank3[code][j]<<" ";
            }
        }

        fout<<endl;
    }

    //时间戳：
    for(int i=1;i<=idx2;i++){
        int id_p=pro_list[i].code;
        fout<<publish_time[id_p]<<endl;
    }


    queue<int> temp=propose;
    fout<<temp.size()<<endl;
    while(!temp.empty()){
        fout<<temp.front()<<" ";
        temp.pop();
    }
    fout << endl;

    fout.close();
    cout<<"数据保存成功！"<<endl;
}


void load_data(){
    ifstream fin("data.txt");

    if(!fin){
        // 第一次运行没有文件是正常的
        return;
    }

    // 清空原有数据
    dep_count=0;
    idx1=0;
    pro_count=0;
    idx2=0;

    depart_idx.clear();
    pro_idx.clear();
    grade_sum.clear();

    // 1. 读取院系
    fin>>dep_count;


    


    for(int i=1;i<=dep_count;i++){
        depart d;
        double sum;

        fin>>d.code>>d.name>>sum;

        dep_list[++idx1]=d;

        depart_idx[d.name]=d.code;
        did_to_name[d.code]=d.name;

        grade_sum[d.name]=sum;
    }

    // 2. 读取项目
    fin>>pro_count;

    for(int i=1;i<=pro_count;i++){
        project p;
        int st;
        string first,second,third;

        fin>>p.code>>p.name>>p.type>>p.score>>st;
        fin>>first>>second>>third;

        pro_list[++idx2]=p;

        string key=p.name+p.type;
        pro_idx[key]=p.code;
        pid_to_name[p.code]=key;
        state[p.code]=st;

        if(first!="NULL"){
            pro_rank3[p.code][1]=first;
            int id_d=depart_idx[first];
            dep_p_r[id_d][p.code]=1;
        }

        if(second!="NULL"){
            pro_rank3[p.code][2]=second;
            int id_d=depart_idx[second];
            dep_p_r[id_d][p.code]=2;
        }

        if(third!="NULL"){
            pro_rank3[p.code][3]=third;
            int id_d=depart_idx[third];
            dep_p_r[id_d][p.code]=3;
        }
    }

    for(int i=1;i<=idx2;i++){
        int id_p=pro_list[i].code;
        fin>>publish_time[id_p];
    }

    int qsize;
    fin>>qsize;

    while(!propose.empty()){
        propose.pop();
    }

    for(int i=0;i<qsize;i++){
        int x;
        fin>>x;
        propose.push(x);
    }

    fin.close();
}




//系数获取：
double get_coe(int rank){
    if(rank==1)return 1;
    else if(rank==2) return 0.5;
    else if(rank==3) return 0.2;
    else return 0;
}


//公示时间计算；
int time_checker(int id_p){
    time_t now=time(nullptr);
    double diff=difftime(now,publish_time[id_p]);
    if(diff<3600){
        return 1; // 公示中
    }
    else{
        state[id_p]=2;
        return 0; // 公示结束
    }

}

//成绩导入：
void input_score(){
    cout<<"请输入项目名称："<<endl;
    string p;
    cin>>p;
    cout<<"请输入项目类型："<<endl;
    string t;
    cin>>t;


    //查找分数
    int id=0;
    for(int i=1;i<=idx2;i++){
        if(pro_list[i].name==p&&pro_list[i].type==t){
            id=i;
            break;
        }
    }
    if(id==0){
        cout<<"无该项目"<<endl;
        return;
    }


    cout<<"请输入院系名称："<<endl;
    string d;cin>>d;

    if(!depart_idx.count(d)){
        cout<<"无该院系"<<endl;
        return;
    }


    
    cout<<"请输入该院系名次："<<endl;
    int r=read_int_range(1,3);

    p=p+t;
    int id_p=pro_idx[p];
    int id_d=depart_idx[d];

    
        
    
    pro_rank3[id_p][r]=d;
    dep_p_r[id_d][id_p]=r;
    
    
    if(pro_rank3[id_p][1]!=""&&pro_rank3[id_p][2]!=""&&pro_rank3[id_p][3]!=""){
        
        state[id_p]=3;
        cout<<"该项目成绩前三名均录入，分别为："<<endl;
        cout<<"第一名："<<pro_rank3[id_p][1]<<endl;
        cout<<"第二名："<<pro_rank3[id_p][2]<<endl;
        cout<<"第三名："<<pro_rank3[id_p][3]<<endl;
        cout<<"----------------------------------"<<endl;
        cout<<"是否立即公示成绩？公示后成绩不可随便修改"<<endl;
        cout<<"输入1代表立即公示，输入0代表有错误，重新录入该项目成绩"<<endl;

        int op = read_int_range(0, 1);
        if(op==1){
            state[id_p]=3;
            for(int i=1;i<=3;i++){
                grade_sum[pro_rank3[id_p][i]]+=get_coe(i)*pro_list[id].score;
            }
            publish_time[id_p]=time(nullptr); // 当前时间戳
            cout<<"成绩已公示！"<<endl;
        }
        else{
            for(int i=1;i<=3;i++){
                pro_rank3[id_p][i]="";
                dep_p_r[id_d][id_p]=0;
            }
            cout<<"该项目数据已删除，请重新录入该项目成绩！"<<endl;
        }
    }
    
}


void del_score(){
    cout<<"请输入项目名称："<<endl;
    string p;
    cin>>p;
    cout<<"请输入项目类型："<<endl;
    string t;
    cin>>t;
    if(!pro_idx.count(p+t)){
        cout<<"无该项目"<<endl;
        return;
    }
    
    if(state[pro_idx[p+t]]==1){
        cout<<"该项目复核申请正在处理中，无法删除！"<<endl;
        return;
    }
    else if(state[pro_idx[p+t]]==2){
        cout<<"该项目成绩已不可修改，无法删除！"<<endl;
        return;
    }
    else if(state[pro_idx[p+t]]==3){
        cout<<"该项目成绩正在公示中，无法删除！"<<endl;
        return;
    }

    cout<<"-------------------------------"<<endl;
    int id_p=pro_idx[p+t];
    for(int i=1;i<=3;i++){
        if(pro_rank3[id_p][i]!="")cout<<"第"<<i<<"名为："<<pro_rank3[id_p][i]<<" ";
    }cout<<endl;
    cout<<"--------------------------------"<<endl;


    cout<<"请输入要删除的名次："<<endl;
    int r=read_int_range(1,3);
    if(pro_rank3[id_p][r]==""){
        cout<<"该名次暂无院系，无法删除！"<<endl;
        return;
    }
    string d=pro_rank3[id_p][r];
    int id_d=depart_idx[d];

    grade_sum[d]-=get_coe(r)*pro_list[id_p].score;
    pro_rank3[id_p][r]="";
    dep_p_r[id_d][id_p]=0;
    state[id_p]=0;
    cout<<"删除成功！"<<endl;


}


//管理员登录系统：
int admin_login(){
    string pwd;
    cout<<"-------管理员登录----------"<<endl;
    cout<<"请输入密码"<<endl;
    cin>>pwd;
    if(pwd=="admin123"){
        cout<<"登录成功"<<endl;
        return 1;
    }
    else{
        cout<<"登录失败"<<endl;
        return 0;
    }
}


//增删查改
void add_dep(){
    if(dep_count>=MAX_DEPT){
        cout<<"院系数量已达上限！"<<endl;
        return;
    }
    

    cout<<"请输入院系编号，编号在0-1000之间"<<endl;
    
    int c=read_int_range(0,1000);
    
    if(c<0||c>1000){
        cout<<"院系编号不合法！"<<endl;
        return;
    }

    if(did_to_name[c]!=""){
        cout<<"该院系编号已存在，不能重复添加！"<<endl;
        return;
    }

    cout<<"请输入院系名称"<<endl;
    string name;cin>>name;
    if(depart_idx.count(name)){
        cout<<"该院系名称已存在，不能重复添加！"<<endl;
        return;
    }

    depart d;
    d.code=c;
    d.name=name;


    depart_idx[name]=c;
    did_to_name[c]=name;

    dep_list[++idx1]=d;
    dep_count++;

    cout<<"添加成功"<<endl;
}


void del_dep(){
    if(dep_count==0){
        cout<<"当前无院系"<<endl;
        return;
    }
    
    cout<<"请输入院系名称"<<endl;
    string name;cin>>name;

    int f=0;
    for(int i=1;i<=idx1;i++){
        if(dep_list[i].name==name){
            f=i;
            break;
        }
    }

    if(f==0){
        cout<<"无该院系"<<endl;
        return;
    }
    else{
        depart_idx.erase(name);
        did_to_name[dep_list[f].code]="";
        grade_sum.erase(name);
        
        for(int i=f;i<idx1;i++){
            dep_list[i]=dep_list[i+1];
        }
        idx1--;
        dep_count--;
        
        cout<<"删除成功！"<<endl;
    }

}


void change_dep(){
    cout<<"请输入需要更改的院系名称"<<endl;
    string s;cin>>s;

    if(!depart_idx.count(s)){
        cout<<"无该院系"<<endl;
        return;
    }

    cout<<"院系编号修改为："<<endl;
    int new_id = read_int_range(0, 1000);
    
    if(new_id<0||new_id>1000){
        cout<<"院系编号不合法！"<<endl;
        return;
    }
    if(did_to_name[new_id]!=""){
        cout<<"该编号已被其他院系使用，修改失败！"<<endl;
        return;
    }

    for(int i=1;i<=idx1;i++){
        if(dep_list[i].name==s){
            int old_id=depart_idx[s];
            did_to_name[old_id]="";

            dep_list[i].code=new_id;
            depart_idx[s]=new_id;
            did_to_name[new_id]=s;

            cout<<"修改成功"<<endl;
            break;
        }
    }
}

//输出各院系
void query_all_depts(){
    cout<<"------院系列表--------"<<endl;
    for(int i=1;i<=idx1;i++){
        cout<<"编号："<<dep_list[i].code<<" "<<"名称："<<dep_list[i].name<<" "<<"总分："<<grade_sum[dep_list[i].name]<<endl;
    }
}

//按总分排序输出各院系
bool cmp(depart a,depart b){
    return grade_sum[a.name]>grade_sum[b.name];
}
void sort_puts(){
    sort(dep_list+1,dep_list+1+idx1,cmp);
    for(int i=1;i<=idx1;i++){
        cout<<"编号："<<dep_list[i].code<<" "<<"名称："<<dep_list[i].name<<" "<<"总分："<<grade_sum[dep_list[i].name]<<endl;
    }
}

//编号为c的院系的总成绩
void ask_grade(){
    cout<<"请输入院系编号"<<endl;
    int c = read_int();
    
    if(c<0 || c>1000 || did_to_name[c]==""){
        cout<<"无该院系"<<endl;
        return;
    }

    cout<<grade_sum[did_to_name[c]];
}

//查项目前三院系
void ask_pro(){
    cout<<"请输入要查找的项目名称"<<endl;
    string name;cin>>name;
    cout<<"请输入要查找的项目类型"<<endl;
    string t;cin>>t;
    name=name+t;
    if(!pro_idx.count(name)){
        cout<<"无该项目"<<endl;
        return;
    }


    int id_p=pro_idx[name];
    

    for(int i=1;i<=3;i++){
        if(pro_rank3[id_p][i]!="")cout<<"第"<<i<<"名为："<<pro_rank3[id_p][i]<<" ";
    }cout<<endl;
}


//加项目：
void add_pro(){
    if(pro_count>=MAX_PRO){
        cout<<"项目数量已达上限！"<<endl;
        return;
    }
    project d;
    cout<<"请输入项目编号"<<endl;
    int c=read_int_range(1,1000);
    


    if(c<=0||c>1000){
        
        cout<<"项目编号必须在1到1000之间！"<<endl;
        return;
    }
    if(pid_to_name[c]!=""){
        cout<<"该项目编号已存在，不能重复添加！"<<endl;
        return;
    }




    d.code=c;
    cout<<"请输入项目名称"<<endl;
    string name;cin>>name;
    d.name=name;
    cout<<"请输入项目类型"<<endl;
    string type;cin>>type;
    d.type=type;

    name=name+type;

    if(pro_idx.count(name)){
        cout<<"该项目名称与类型的组合已存在，不能重复添加！"<<endl;
        return;
    }


    cout<<"请输入项目分数"<<endl;
    double score = read_double();
    d.score=score;

    pro_list[++idx2]=d;

    
    pro_idx[name]=c;
    pid_to_name[c]=name;

    pro_count++;
    cout<<"添加成功"<<endl;
}


void del_pro(){
    if(pro_count==0){
        cout<<"当前无项目"<<endl;
        return;
    }

    cout<<"请输入项目名称"<<endl;
    string name;cin>>name;
    cout<<"请输入项目类型"<<endl;
    string t;cin>>t;
    
    int f=0;
    for(int i=1;i<=idx2;i++){
        if(pro_list[i].name==name&&pro_list[i].type==t){
            f=i;
            break;
        }
    }

    if(f==0){
        cout<<"无该项目"<<endl;
        return;
    }
    else{
        pid_to_name[pro_list[f].code]="";
        pro_idx.erase(name+t);

        for(int i=f;i<idx2;i++){
            pro_list[i]=pro_list[i+1];
        }

        int id_p=pro_list[f].code;
        if(pro_rank3[id_p][1]!=""&&pro_rank3[id_p][2]!=""&&pro_rank3[id_p][2]!=""){
            for(int i=1;i<=3;i++){
                grade_sum[pro_rank3[id_p][i]]-=get_coe(i)*pro_list[f].score;
            }
            state[id_p]=0;
        }

        idx2--;
        pro_count--;
        cout<<"删除成功！"<<endl;
    }
}


void change_pro(){
    cout<<"请输入需要更改的项目名称"<<endl;
    string s;cin>>s;
    cout<<"请输入需要更改的项目类型"<<endl;
    string t;cin>>t;

    if(!pro_idx.count(s+t)){
        cout<<"无该项目"<<endl;
        return;
    }

    

    int old_id=pro_idx[s+t];
    for(int i=1;i<=idx2;i++){
        if(pro_list[i].name==s&&pro_list[i].type==t){
            cout<<"项目编号修改为："<<endl;
            int new_id = read_int_range(1, 1000);

            if(new_id<=0||new_id>1000){
                cout<<"项目编号必须在1到1000之间！"<<endl;
                return;
            }
            if(new_id!=old_id&&pid_to_name[new_id]!=""){
                cout<<"该项目编号已被其他项目使用，修改失败！"<<endl;
                return;
            }

            
            pid_to_name[old_id]="";

            pro_list[i].code=new_id;
            pro_idx[s+t]=new_id;
            pid_to_name[new_id]=s+t;

            cout<<"修改成功"<<endl;
            break;
        }
    }
}


void change_grade(int id_p,int id){
    cout<<"请输入院系名称："<<endl;
    string d;
    cin>>d;

    if(!depart_idx.count(d)){
        cout<<"无该院系"<<endl;
        return;
    }


    cout<<"请输入该院系名次："<<endl;
    int r = read_int_range(1, 3);
    
    
    if(pro_rank3[id_p][r]!=""){
        cout<<"该名次已有院系，修改失败！"<<endl;
        return;
    }
    
    int id_d=depart_idx[d];
    pro_rank3[id_p][r]=d;
    grade_sum[d]+=get_coe(r)*pro_list[id].score;
    dep_p_r[id_d][id_p]=r;
}

void appeal(){
    if(propose.size()!=0){
        cout<<"您目前有"<<propose.size()<<"条复核申请,请尽快处理！"<<endl;
    }
    else{
        cout<<"当前无复核申请！"<<endl;
    }
    while(propose.size()){
        auto t=propose.front();
        propose.pop();

        if(!time_checker(t)){
            cout<<"项目"<<pid_to_name[t]<<"成绩公示已结束，修改时间已过期"<<endl;
            state[t]=2;
            continue;
        } // 检查是否公示结束，公示结束则不允许修改
        

        cout<<"输入1代表同意复核申请，输入0代表不同意复核申请，维持原成绩"<<endl;
        int op = read_int_range(0, 1);
        if(op==0){
            state[t]=2;
            continue;
        }
        else{
            int id_p=t;
            cout<<"已清除该项目成绩，请重新输入该项目修改后的三个名次"<<endl;

            //清除原来的项目得分,与名次

            //找项目
            int id=0;
            for(int i=1;i<=idx2;i++){
                if(pro_idx[pro_list[i].name+pro_list[i].type]==id_p){
                    id=i;
                    break;
                }
            }
            //消除分数
            for(int i=1;i<=3;i++){
                grade_sum[pro_rank3[id_p][i]]-=get_coe(i)*pro_list[id].score;
            }
            //消除排名
            for(int i=1;i<=3;i++){
                int idd=depart_idx[pro_rank3[id_p][i]];
                dep_p_r[idd][id_p]=0;
                pro_rank3[id_p][i]="";
            }
            //输入三个成绩
            while(pro_rank3[id_p][1]==""||pro_rank3[id_p][2]==""||pro_rank3[id_p][3]==""){
                change_grade(id_p,id);
            }
            state[id_p]=2;
            cout<<"复核处理完成！"<<endl;
        }
    }
}


void query_pro(){
    cout<<"--------项目列表-----------"<<endl;
    for(int i=1;i<=idx2;i++){
        cout<<"编号："<<pro_list[i].code<<" "<<"名称："<<pro_list[i].name<<" "<<"类型："<<pro_list[i].type<<" "<<"分数："<<pro_list[i].score<<endl;
    }
}


//普通用户：

int user_login(){
    string pwd;
    cout<<"-------普通用户登录----------"<<endl;
    cout<<"请输入密码"<<endl;
    cin>>pwd;
    if(pwd=="user_123"){
        cout<<"登录成功"<<endl;
        return 1;
    }
    else{
        cout<<"登录失败"<<endl;
        return 0;
    }
}

void user_ask_dept_rank(){
    cout<<"请输入项目名称："<<endl;
    string p;
    cin>>p;
    cout<<"请输入项目类型："<<endl;
    string t;
    cin>>t;

    int id_p=pro_idx[p+t];
    if(state[id_p]==0){
        cout<<"该项目成绩暂未公示"<<endl;
        return;
    }

    if(!pro_idx.count(p+t)){
        cout<<"无该项目"<<endl;
        return;
    }
    
    cout<<"请输入院系名称："<<endl;
    string d;
    cin>>d;
    
    if(!depart_idx.count(d)){
        cout<<"无该院系"<<endl;
        return;
    }
    
    p=p+t;
    
    int id_d=depart_idx[d];
    
    cout<<"排名为："<<dep_p_r[id_d][id_p]<<endl;
}

void user_review(){
    cout<<"请输入您想复核的项目名称" <<endl;
    string name;cin>>name;
    cout<<"请输入您想复核的项目类型"<<endl;
    string t;cin>>t;
    if(!pro_idx.count(name+t)){
        cout<<"无该项目"<<endl;
        return;
    }


    int id_p=pro_idx[name+t];

    if(!time_checker(id_p)){
        cout<<"该项目成绩公示已结束，暂不接受复核申请"<<endl;
        state[id_p]=2;
        return;
    }

    if(state[id_p]==0){
        cout<<"该项目成绩暂未公示，暂不接受复核申请"<<endl;
        return;
    }

    if(state[id_p]==2){
        cout<<"该项目成绩已不可修改！不再接受申诉"<<endl;
        return;
    }

    if(state[id_p]==1){
        cout<<"该项目复核申请已提交，请勿重复提交"<<endl;
        return;
    }

    state[id_p]=1;
    propose.push(id_p);
    cout<<"复核申请已提交"<<endl;
}

void review_check(){
    cout<<"请输入您想复核的项目名称" <<endl;
    string name;cin>>name;
    cout<<"请输入您想复核的项目类型"<<endl;
    string t;cin>>t;
    if(!pro_idx.count(name+t)){
        cout<<"无该项目"<<endl;
        return;
    }


    int id_p=pro_idx[name+t];
    if(state[id_p]==1){
        cout<<"您的复核申请正在处理中，请您稍后查看！"<<endl;
    }
    else if(state[id_p]==2){
        cout<<"您的复核申请已处理，请您重新前往查询成绩！"<<endl;
    }
    else if(state[id_p]==0){
        cout<<"该项目成绩暂未公示，无需复核！"<<endl;
    }
    else if(state[id_p]==3){
        cout<<"该项目成绩正在公示中，无需复核！"<<endl;
    }
    
}



int main(){
    /*ios::sync_with_stdio(false);
    cin.tie(0);*/
    load_data();

    cout<<"欢迎进入运动会积分管理系统！"<<endl;
    start:
    cout<<"输入1，管理员登录，输入0，普通用户登录"<<endl;
    int op=read_int_range(0,1);
    
    if(op==1){
        int f=admin_login();
        if(f==0){
            cout<<"输入错误！"<<endl;
            goto start;
        }
        else{
            while(1){
                cout<<"------------------------------"<<endl;
                cout<<"增加项目输入1"<<endl;
                cout<<"删除项目输入2"<<endl;
                cout<<"修改项目输入3"<<endl;
                cout<<"增加院系输入4"<<endl;
                cout<<"删除院系输入5"<<endl;
                cout<<"修改院系输入6"<<endl;
                cout<<"输出各院系总成绩输入7"<<endl;
                cout<<"排序后输出各院系总成绩输入8"<<endl;
                cout<<"奖项录入输入9"<<endl;
                cout<<"复核处理输入10"<<endl;
                cout<<"按系院编号查询成绩输入11"<<endl;
                cout<<"按项目编号查询前三名输入12"<<endl;
                cout<<"输出已存入项目输入13"<<endl;
                cout<<"保存数据输入14"<<endl;
                cout<<"奖项删除输入15"<<endl;
                cout<<"退出系统输入0"<<endl;
                cout<<"------------------------------"<<endl;
                int op=read_int_range(0,15);
                if(op==1)add_pro();
                else if(op==2)del_pro();
                else if(op==3)change_pro();
                else if(op==4)add_dep();
                else if(op==5)del_dep();
                else if(op==6)change_dep();
                else if(op==7)query_all_depts();
                else if(op==8)sort_puts();
                else if(op==9)input_score();
                else if(op==10)appeal();
                else if(op==11)ask_grade();
                else if(op==12)ask_pro();
                else if(op==13)query_pro();
                else if(op==14)save_data();
                else if(op==15){
                    del_score();
                }
                else if(op==0){
                    save_data();
                    return 0;
                }
                else{
                    cout<<"输入错误，请重新输入！"<<endl;
                }
            }
        }
    }
    else if(op==0){
        int f=user_login();
        if(f==0){
            cout<<"输入错误！"<<endl;
            goto start;
        }
        else{
            while(1){
                cout<<"查询院系比赛项目名次输入1"<<endl;
                cout<<"生成团体总分报表输入2"<<endl;
                cout<<"提出复核申请输入3"<<endl;
                cout<<"查看复核状态输入4"<<endl;
                cout<<"退出系统输入5"<<endl;
                int op = read_int_range(1, 5);
                if(op==1)user_ask_dept_rank();
                else if(op==2)query_all_depts();
                else if(op==3)user_review();
                else if(op==4)review_check();
                else if(op==5){
                    save_data();
                    return 0;
                }    
                else{
                    cout<<"输入错误，请重新输入！"<<endl;
                }
            }
        }
    }


    return 0;
}
