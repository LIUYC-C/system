#include<iostream>
#include<algorithm>
#include<math.h>
#include<unordered_map>
#include<queue>
using namespace std;

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

//各项目状态 0为公示 1为待复核状态 2为不允许修改：
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




//系数获取：
double get_coe(int rank){
    if(rank==1)return 1;
    else if(rank==2) return 0.5;
    else if(rank==3) return 0.2;
    else return 0;
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
    string d;
    cin>>d;
    cout<<"请输入该院系名次："<<endl;
    int r;
    cin>>r;
    while(r<1||r>3){
        cout<<"名次为一到三名 请重新输入"<<endl;
        cin>>r;
    }

    p=p+t;
    int id_p=pro_idx[p];
    int id_d=depart_idx[d];

    
    pro_rank3[id_p][r]=d;
    
    grade_sum[d]+=get_coe(r)*pro_list[id].score;
    
    dep_p_r[id_d][id_p]=r;

    state[id_p]=0;
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
    depart d;

    cout<<"请输入院系编号"<<endl;
    int c;cin>>c;
    d.code=c;
    cout<<"请输入院系名称"<<endl;
    string name;cin>>name;
    d.name=name;


    depart_idx[name]=c;
    dep_list[++idx1]=d;
    did_to_name[c]=name;


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
    for(int i=1;i<=idx1;i++){
        if(dep_list[i].name==s){
            cout<<"院系编号修改为："<<endl;
            int id;
            cin>>id;
            dep_list[i].code=id;
            depart_idx[s]=id;
            did_to_name[id]=s;
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
    int c;cin>>c;
    cout<<grade_sum[did_to_name[c]];
}

//查项目前三院系
void ask_pro(){
    cout<<"请输入要查找的项目名称"<<endl;
    string name;cin>>name;
    cout<<"请输入要查找的项目类型"<<endl;
    string t;cin>>t;
    name=name+t;
    int id_p=pro_idx[name];
    for(int i=1;i<=3;i++){
        cout<<pro_rank3[id_p][i]<<" ";
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
    int c;cin>>c;
    d.code=c;
    cout<<"请输入项目名称"<<endl;
    string name;cin>>name;
    d.name=name;
    cout<<"请输入项目类型"<<endl;
    string type;cin>>type;
    d.type=type;
    cout<<"请输入项目分数"<<endl;
    double score;cin>>score;
    d.score=score;

    pro_list[++idx2]=d;

    name=name+type;
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
        for(int i=f;i<idx2;i++){
            pro_list[i]=pro_list[i+1];
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
    for(int i=1;i<=idx2;i++){
        if(pro_list[i].name==s&&pro_list[i].type==t){
            cout<<"项目编号修改为："<<endl;
            int id;
            cin>>id;
            pro_list[i].code=id;
            pro_idx[s+t]=id;
            pid_to_name[id]=s+t;
            cout<<"修改成功"<<endl;
            break;
        }
    }
}


void change_grade(int id_p,int id){
    cout<<"请输入院系名称："<<endl;
    string d;
    cin>>d;
    cout<<"请输入该院系名次："<<endl;
    int r;
    cin>>r;

    int id_d=depart_idx[d];
    
    pro_rank3[id_p][r]=d;
    grade_sum[d]+=get_coe(r)*pro_list[id].score;
    dep_p_r[id_d][id_p]=r;
}

void appeal(){
    if(propose.size()!=0){
        cout<<"您目前有"<<propose.size()<<"条复核申请,请尽快处理！"<<endl;
    }
    while(propose.size()){
        auto t=propose.front();
        propose.pop();
        cout<<"输入1代表同意复核申请，输入0代表不同意复核申请，维持原成绩"<<endl;
        int op;cin>>op;
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
            }
            //输入三个成绩
            change_grade(id_p,id);
            change_grade(id_p,id);
            change_grade(id_p,id);
            state[id_p]=2;
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
    cout<<"请输入院系名称："<<endl;
    string d;
    cin>>d;
    p=p+t;
    int id_p=pro_idx[p];
    int id_d=depart_idx[d];
    
    cout<<"排名为："<<dep_p_r[id_d][id_p]<<endl;
}

void user_review(){
    cout<<"请输入您想复核的项目名称" <<endl;
    string name;cin>>name;
    cout<<"请输入您想复核的项目类型"<<endl;
    string t;cin>>t;
    
    int id_p=pro_idx[name+t];
    if(state[id_p]==2){
        cout<<"该项目成绩已不可修改！不再接受申诉"<<endl;
        return;
    }

    state[id_p]=1;
    propose.push(id_p);
}

void review_check(){
    cout<<"请输入您想复核的项目名称" <<endl;
    string name;cin>>name;
    cout<<"请输入您想复核的项目类型"<<endl;
    string t;cin>>t;

    int id_p=pro_idx[name+t];
    if(state[id_p]==1){
        cout<<"您的复核申请正在处理中，请您稍后查看！"<<endl;
    }
    else if(state[id_p]==2){
        cout<<"您的复核申请已处理，请您重新前往查询成绩！"<<endl;
    }
}



int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    start:
    cout<<"欢迎进入运动会积分管理系统！"<<endl;
    cout<<"输入1，管理员登录，输入0，普通用户登录"<<endl;
    int op;cin>>op;
    if(op==1){
        int f=admin_login();
        if(f==0){
            goto start;
        }
        else{
            while(1){
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
                cout<<"退出系统输入0"<<endl;
                int op;cin>>op;
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
                else if(op==0){
                    return 0;
                }
            }
        }
    }
    else if(op==0){
        int f=user_login();
        if(f==0){
            goto start;
        }
        else{
            while(1){
                cout<<"查询院系比赛项目名次输入1"<<endl;
                cout<<"生成团体总分报表输入2"<<endl;
                cout<<"提出复核申请输入3"<<endl;
                cout<<"查看复核状态输入4"<<endl;
                cout<<"退出系统输入5"<<endl;
                int op;cin>>op;
                if(op==1)user_ask_dept_rank();
                else if(op==2)query_all_depts();
                else if(op==3)user_review();
                else if(op==4)review_check();
                else{
                    return 0;
                }    
            }
        }
    }


    return 0;
}
