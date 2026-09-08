/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      admin
Version:     1.0
Date:        2018-09-20 09:51:36
Description: 数据表结构读取函数
**************************************************/

#include "CUtils.h"

BM2_FUNCTION_EXPORT
int f_gctp_getTableItem(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn, int table_type, CString table_name)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CString querySql = "";
	CString tableNameC = "";
	CString cfgitmGrpName = "";
	CDbCommand cmd_inq(conn);
	list<CString> listPkColName;

	try
	{
		//table_type 0:实体单表; 1:实体联合表(在线+历史); 2:虚拟表; 3:电文表
		//10:配置表(配置主项); 11:配置表(主项字段);12:配置表(条件主项); 13:配置表(条件字段); 
		//14:配置表(Grid主项); 15:配置表(Grid字段); 16:配置表(明细主项); 17:配置表(明细字段); 
		//18:框架按钮; 
		//19:配置表(自定义事件); //20:配置表(画面操作); 21:配置表(输入数据); 22:配置表(输出数据); 
		PrintLog("table_type", table_type);
		PrintLog("table_name", table_name);
		if (table_type < 10 && table_name.Trim() == "")
		{
			return doFlag;
		}

		if (bcls_ret->Tables.get_Count() == 1 && bcls_ret->Tables[0].get_TableName() == "Table0")
		{
			PrintLog("0");
			bcls_ret->Tables[0].set_TableName("TABLE");
		}

		if (!bcls_ret->Tables.Contains("TABLE"))
		{
			bcls_ret->Tables.Add("TABLE");
		}

		//实体联合表转换成在线表表名
		if (table_type == 1 && table_name.Substring(0, 1).ToUpper() != "T")
		{
			table_name = "T" + table_name.Substring(1);
		}

		switch (table_type)
		{
		case 0:
		case 1:
			//返回数据字典中的表列数据
			bcls_ret->Tables["TABLE"] = GetTableColName(table_name, conn);
			bcls_ret->Tables["TABLE"].Columns.Add(DT_STRING, "TABLE_ENAME");
			bcls_ret->Tables["TABLE"].Columns.Add(DT_DECIMAL, "SEQ_NO");
			bcls_ret->Tables["TABLE"].Columns.Add(DT_STRING, "ITEM_KIND");
			bcls_ret->Tables["TABLE"].Columns.Add(DT_STRING, "PK_FLAG");
			bcls_ret->Tables["TABLE"].Columns.Add(DT_STRING, "NOW_ROW");

			listPkColName = GetTablePkColName(table_name, conn);
			for (list<CString>::const_iterator iter = listPkColName.begin(); iter != listPkColName.end(); iter++)
			{
				for (int i = 0; i < bcls_ret->Tables["TABLE"].Rows.get_Count(); i++)
				{
					if (*iter == bcls_ret->Tables["TABLE"].Rows[i]["COLUMN_NAME"].ToString())
					{
						bcls_ret->Tables["TABLE"].Rows[i]["PK_FLAG"] = "1";
					}

					bcls_ret->Tables["TABLE"].Rows[i]["TABLE_ENAME"] = table_name;
					bcls_ret->Tables["TABLE"].Rows[i]["SEQ_NO"] = i + 1;
					if (bcls_ret->Tables["TABLE"].Rows[i]["DATA_TYPE"].ToString() == "C")
					{
						bcls_ret->Tables["TABLE"].Rows[i]["ITEM_KIND"] = "S";
					}
					else
					{
						bcls_ret->Tables["TABLE"].Rows[i]["ITEM_KIND"] = "D";
					}

					bcls_ret->Tables["TABLE"].Rows[i]["NOW_ROW"] = CDecimal(i).ToString();
				}
			}

			//添加表名中文描述
			switch (conn->DatabaseKind)
			{
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
				break;
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
// DM8 适配 CHANGE-361:查询表名中文描述,从系统目录 SYSIBM.SYSTABLES 取 remarks(按表名与当前模式过滤)。
// 改写原因：SYSIBM 辅助表改为 DUAL；DB2 特殊寄存器 current date/time/timestamp/schema 改为 DM 的 CURRENT_DATE/CURRENT_TIME/CURRENT_TIMESTAMP/CURRENT_SCHEMA(官方函数手册支持)；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
				// querySql = "SELECT remarks FROM SYSIBM.SYSTABLES WHERE name = '" + table_name +
					// "' AND creator = (SELECT current schema FROM sysibm.sysdummy1)";
// DM8 SQL：
				querySql = "SELECT remarks FROM SYSIBM.SYSTABLES WHERE name = '" + table_name +
					"' AND creator = (SELECT CURRENT_SCHEMA FROM DUAL)";
				break;
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				querySql = "SELECT comments FROM user_tab_comments WHERE table_name = '" + table_name + "'";
				break;
			}

			cmd_inq.SetCommandText(querySql);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tableNameC = cmd_inq.GetString(1);
			}
			cmd_inq.Close();

			break;
		case 2:
			sqlstr = "SELECT * FROM tgctp10 WHERE table_ename = '" + table_name + "'";
			break;
		case 3:
			querySql = "SELECT tc_name FROM text1_res WHERE tc_no = '" + table_name + "' AND culture = 'zh_Hans'";
			cmd_inq.SetCommandText(querySql);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tableNameC = cmd_inq.GetString(1);
			}
			cmd_inq.Close();

			sqlstr = "SELECT UPPER(t1.tc_item_name) AS column_name,tc_item_desc AS column_cname,DECODE(tc_item_type,'C','C','N')"
				" AS data_type,DECODE(tc_item_dec,0,to_char(tc_item_len),to_char(tc_item_len) || to_char(tc_item_dec)) AS data_length "
				" FROM text2 t1,text2_res t2 WHERE t1.tc_no = '" + table_name + "' AND t1.tc_no = t2.tc_no"
				" AND t1.tc_item_name = t2.tc_item_name AND t2.culture = 'zh_Hans' ORDER BY t1.tc_item_seq_no";
			break;
		case 10:
			cfgitmGrpName = "DS_MAIN";
			break;
		case 11:
			cfgitmGrpName = "DS_ITEM";
			break;
		case 12:
			cfgitmGrpName = "CONDITION_MAIN";
			break;
		case 13:
			cfgitmGrpName = "CONDITION_ITEM";
			break;
		case 14:
			cfgitmGrpName = "GRID_MAIN";
			break;
		case 15:
			cfgitmGrpName = "GRID_ITEM";
			break;
		case 16:
			cfgitmGrpName = "CONTROL_MAIN";
			break;
		case 17:
			cfgitmGrpName = "CONTROL_ITEM";
			break;
		case 18:
			cfgitmGrpName = "FORM_CFGITM";
			break;
		case 19:
			cfgitmGrpName = "CUSTOM_EVENT";
			break;
		case 20:
			cfgitmGrpName = "OPERATE_DO";
			break;
		case 21:
			cfgitmGrpName = "OPERATE_IN";
			break;
		case 22:
			cfgitmGrpName = "OPERATE_OUT";
			break;
		default:
			break;
		}

		if (cfgitmGrpName.Trim() != "")
		{
			sqlstr = "SELECT * FROM tgctp00 WHERE cfgitm_grp_name = '" + cfgitmGrpName + "' ORDER BY seq_no";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables["TABLE"]);
			cmd_inq.Close();
		}
		else if (sqlstr.Trim() != "")
		{
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables["TABLE"]);
			cmd_inq.Close();
		}

		AddColValue(bcls_ret->Tables["TABLE"], "TABLE_NAME", table_name);
		if (tableNameC.Trim() != "")
		{
			AddColValue(bcls_ret->Tables["TABLE"], "TABLE_CNAME", tableNameC);
		}

		PrintLog("sqlstr", sqlstr);
		//PrintDataTable(bcls_ret->Tables[0]);
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}

BM2_FUNCTION_EXPORT
int f_gctp_getTableItem(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	int tableType = 0;
	CString tableName = "";

	try
	{
		tableType = GetColValueD(bcls_rec->Tables[0], 0, "TABLE_TYPE").ToInt32();
		tableName = GetColValueC(bcls_rec->Tables[0], 0, "TABLE_NAME");

		doFlag = f_gctp_getTableItem(bcls_rec, bcls_ret, conn, tableType, tableName);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
