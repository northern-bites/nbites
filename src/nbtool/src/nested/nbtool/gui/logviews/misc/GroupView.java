package nbtool.gui.logviews.misc;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.nio.file.Path;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JOptionPane;
import javax.swing.JPanel;

import nbtool.data.group.AllGroups;
import nbtool.data.group.Group;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.gui.PathChooser;
import nbtool.gui.ToolMessage;
import nbtool.nio.FileIO;
import nbtool.util.Debug;
import nbtool.util.Events;

public class GroupView extends JPanel implements ActionListener {

	@Override
	public void actionPerformed(ActionEvent e) {

		String suggest = null;
		if (group.source == Group.GroupSource.ROBOT_STREAM) {
			String rname = null;
			boolean same = true;

			for (LogReference ref : group.logs) {
				if (rname == null) {
					rname = ref.host_name;
				} else {
					if (!rname.equals(ref.host_name)) {
						same = false;
						break;
					}
				}
			}

			suggest = (same && rname != null) ?
					PathChooser.suggestion(rname) :
						null;
		}

		Path path = PathChooser.chooseDirPath(this, suggest);

		if (path != null) {
			for (LogReference ref : group.logs) {
				Debug.info("writing log r%d to %s", ref.thisID, path);
				ref.copyLogToPath(path);
			}
		}
	}

	private Group group = null;

	public GroupView(Group group) {
		super();
		this.group = group;
		initComponents();

		this.groupInfoLabel.setText(String.format("%s", group));
		HashSet<String> types = new HashSet<>();
		for (LogReference ref : group.logs)
			types.add(ref.logClass);

		this.groupContentsLabel.setText(String.format("%d logs, types: %s",
				group.logs.size(), types));

		final GroupView finalThis = this;
		this.saveButton.addActionListener(this);
		this.addFromGroupButton.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				Group[] possible = AllGroups.allGroups.toArray(new Group[0]);

				Group chosen = (Group) JOptionPane.showInputDialog(finalThis,
						"choose a group", "add logs from:", JOptionPane.PLAIN_MESSAGE,
						null, possible, AllGroups.latestGroup());

				if (chosen != null) {
					List<Log> copied = new LinkedList<>();
					for (LogReference ref : chosen.logs) {
						copied.add(ref.get().deepCopy());
					}

					for (Log l : copied) {
						finalThis.group.add(LogReference.referenceFromLog(l));
					}

					Events.GLogsFound.generate(this, copied.toArray(new Log[0]));
				}
			}

		});

		this.addFromPathButton.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				Path dir = PathChooser.chooseDirPath(finalThis, null);

				if (dir != null) {
					try {
						finalThis.group.add(FileIO.readAllRefsFromPath(dir, false));
					} catch (IOException e1) {
						ToolMessage.displayError("failed to read refs from %s (%s)",
								dir, e1.getMessage());
						e1.printStackTrace();
					}
				}
			}

		});
	}

	/**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">
    private void initComponents() {

        groupInfoLabel = new javax.swing.JLabel();
        groupContentsLabel = new javax.swing.JLabel();
        saveButton = new javax.swing.JButton();
        addFromGroupButton = new javax.swing.JButton();
        addFromPathButton = new javax.swing.JButton();

        groupInfoLabel.setText("jLabel1");

        groupContentsLabel.setText("jLabel2");

        saveButton.setText("save to directory");

        addFromGroupButton.setText("add from group");

        addFromPathButton.setText("add from directory");

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(addFromPathButton, javax.swing.GroupLayout.DEFAULT_SIZE, 436, Short.MAX_VALUE)
            .addComponent(addFromGroupButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(saveButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(groupInfoLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(groupContentsLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(groupInfoLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(groupContentsLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(saveButton, javax.swing.GroupLayout.PREFERRED_SIZE, 64, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(addFromPathButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(addFromGroupButton)
                .addContainerGap(50, Short.MAX_VALUE))
        );
    }// </editor-fold>


    // Variables declaration - do not modify
    private javax.swing.JButton addFromGroupButton;
    private javax.swing.JButton addFromPathButton;
    private javax.swing.JLabel groupContentsLabel;
    private javax.swing.JLabel groupInfoLabel;
    private javax.swing.JButton saveButton;
    // End of variables declaration
}
