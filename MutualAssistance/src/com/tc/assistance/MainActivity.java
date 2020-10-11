package com.tc.assistance;

import java.io.Serializable;
import java.util.List;

import android.app.ExpandableListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.ContextMenu;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.view.ViewGroup;
import android.view.ContextMenu.ContextMenuInfo;
import android.widget.AbsListView;
import android.widget.BaseExpandableListAdapter;
import android.widget.ExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ExpandableListView.ExpandableListContextMenuInfo;

import com.tc.assistance.client.HttpServer;
import com.tc.assistance.entity.device.Relationship;

// ExpandableList - ��չ��/�����б�   
// �̳� ExpandableListActivity ��ʵ���б�Ŀ�չ��/�����Ĺ���   
public class MainActivity extends ExpandableListActivity {

	private ExpandableListAdapter mAdapter;

	protected void onCreate(Bundle savedInstanceState) {

		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);

		setTitle("��������");

		mAdapter = new MyExpandableListAdapter(AccountManager.getInstance());
		setListAdapter(mAdapter);
		this.getExpandableListView().setBackgroundResource(R.drawable.back);
	
		registerForContextMenu(this.getExpandableListView());

	}

	// Ϊ�б��ÿһ��������Ĳ˵���������������Ĳ˵���

	public void onCreateContextMenu(ContextMenu menu, View v,
			ContextMenuInfo menuInfo) {
		ExpandableListContextMenuInfo info = (ExpandableListContextMenuInfo) menuInfo;

		Object o = getObject(info);
		if (o instanceof Relationship) {
			Relationship r = (Relationship) o;
		if (!r.cared) {

			// menu.setHeaderTitle("ContextMenu");
			menu.add(0, 0, 0, "����");
		}
		if (r.cared) {
			// menu.setHeaderTitle("ContextMenu");
			menu.add(0, 0, 0, "�鿴");
		}
		}

	}
	
	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		boolean v = HttpServer.getInstance().getServerConfig().valid();
		if( register != null)
			register.setEnabled(v);
		
		if( downloadGraph != null)
			downloadGraph.setEnabled(v);
	}

	MenuItem login;
	MenuItem register;
	MenuItem downloadGraph;
	MenuItem cared;
	MenuItem caring;
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		login = menu.add("�˺�����");
		register = menu.add("�豸ע��");

		
		downloadGraph = menu.add("��ϵ����");
	

		SubMenu sm = menu.addSubMenu("�豸��ϵ����");
		cared = sm.add("���ӱ����豸");
		caring = sm.add("���ӿ����豸");
		
		
		boolean v = HttpServer.getInstance().getServerConfig().valid();
		if( register != null)
			register.setEnabled(v);
		
		if( downloadGraph != null)
			downloadGraph.setEnabled(v);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		int i = item.getItemId();
		if(login == item){
			 Intent main = new Intent();
             main.setClass(this, LoginActivity.class);
             startActivity(main);
		}
		if(cared == item){
			 Intent main = new Intent();
			 main.putExtra("cared", true);
			 main.setClass(this, RelationActivity.class);
			 startActivity(main);
		}
		if(caring == item){
			Intent main = new Intent();
			main.putExtra("cared", false);
			main.setClass(this, RelationActivity.class);
			startActivity(main);
		}
		if(register == item){
			AccountManager.getInstance().uploadDefination();
			Toast.makeText(this, "�豸�Ѿ��ɹ�ע��", Toast.LENGTH_SHORT).show();
		}
		if(downloadGraph == item){
			AccountManager.getInstance().downloadRelation(this);
			Toast.makeText(this, "�豸��ϵ�Ѿ��ɹ�����", Toast.LENGTH_SHORT).show();
		}
		return true;
	}

	Object getObject(ExpandableListContextMenuInfo info) {
		int type = ExpandableListView
				.getPackedPositionType(info.packedPosition);
		if (type == ExpandableListView.PACKED_POSITION_TYPE_CHILD) {
			int groupPos = ExpandableListView
					.getPackedPositionGroup(info.packedPosition);
			int childPos = ExpandableListView
					.getPackedPositionChild(info.packedPosition);
			Object o = this.getExpandableListAdapter().getChild(groupPos,
					childPos);
			return o;
		}
		return null;
	}

	// ���������Ĳ˵�����߼�

	public boolean onContextItemSelected(MenuItem item) {
		ExpandableListContextMenuInfo info = (ExpandableListContextMenuInfo) item
				.getMenuInfo();
		Object o = getObject(info);
		if (o instanceof Relationship) {
			Relationship r = (Relationship) o;
			if (r.cared) {
				Intent intent = new Intent();
				intent.putExtra("entity", (Serializable) o);
				intent.setClass(this, CaredActivity.class);
				this.startActivity(intent);
			}
		}
		return true;
	}

	public class MyExpandableListAdapter extends BaseExpandableListAdapter {
		AccountManager accountManager;

		public MyExpandableListAdapter(AccountManager am) {
			accountManager = am;
		}

		public Object getChild(int groupPosition, int childPosition) {
			List<Relationship> cs = null;
			if (groupPosition == 0)
				cs = this.accountManager.getAccounts().in;
			else
				cs = this.accountManager.getAccounts().out;
			return cs.get(childPosition);
		}

		public long getChildId(int groupPosition, int childPosition) {
			return childPosition;
		}

		public int getChildrenCount(int groupPosition) {
			if(accountManager.getAccounts() == null)
				return 0;
			if (groupPosition == 0)
				return this.accountManager.getAccounts().in.size();
			else
				return this.accountManager.getAccounts().out.size();
		}

		// ȡ���б��е�ĳһ��� View

		public View getChildView(int groupPosition, int childPosition,
				boolean isLastChild, View convertView, ViewGroup parent) {
			TextView textView = getGenericView();
			textView.setText(getChild(groupPosition, childPosition).toString());
			return textView;
		}

		public Object getGroup(int groupPosition) {
			return this.accountManager.getGroup()[groupPosition];
		}

		public int getGroupCount() {
			return 2;
		}

		public long getGroupId(int groupPosition) {
			return groupPosition;
		}

		// ȡ���б��е�ĳһ��� View

		public View getGroupView(int groupPosition, boolean isExpanded,
				View convertView, ViewGroup parent) {
			TextView textView = getGenericView();
			textView.setText(getGroup(groupPosition).toString());
			return textView;
		}

		public boolean hasStableIds() {
			return true;
		}

		public boolean isChildSelectable(int groupPosition, int childPosition) {
			return true;
		}

		// ��ȡĳһ��� View ���߼�
		private TextView getGenericView() {
			AbsListView.LayoutParams lp = new AbsListView.LayoutParams(
					ViewGroup.LayoutParams.FILL_PARENT, 48);
			TextView textView = new TextView(MainActivity.this);
			textView.setLayoutParams(lp);
			textView.setGravity(Gravity.CENTER_VERTICAL | Gravity.LEFT);
			textView.setPadding(32, 0, 0, 0);
			return textView;
		}
	}
}
