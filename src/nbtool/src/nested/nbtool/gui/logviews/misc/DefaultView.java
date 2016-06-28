package nbtool.gui.logviews.misc;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.io.IOException;
import java.nio.file.Path;
import java.util.LinkedList;
import java.util.List;

import javax.swing.text.DefaultCaret;

import nbtool.data.log.Block;
import nbtool.gui.PathChooser;
import nbtool.gui.ToolMessage;
import nbtool.nio.FileIO;
import nbtool.util.Debug;
import nbtool.util.ToolSettings;

public class DefaultView extends ViewParent implements ActionListener {

	@Override
	public String[] displayableTypes() {
		return new String[]{ToolSettings.DEFAULT_S};
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		Path path = PathChooser.chooseLogPath(this, null);
		if (path == null) return;

		if (!path.toString().endsWith(".nblog")) {
			path = FileIO.getPath(path.toString() + ".nblog");
		}

		try {
			FileIO.writeLogToPath(path, displayedLog);
		} catch (IOException e1) {
			e1.printStackTrace();
			ToolMessage.displayError("write failed for: %s", displayedLog);
		}
	}

	int jsonAreaLoc = 0;

	private void reDisplay() {
		this.classAndHostLabel.setText(
				String.format("logClass= %s from %s@%s (type=%s)"
				, displayedLog.logClass, displayedLog.host_name, displayedLog.host_addr, displayedLog.host_type));

		int tbytes = 0;
		List<String> blockList = new LinkedList<>();
		for (Block b : displayedLog.blocks) {
			tbytes += b.data.length;
			blockList.add(String.format("type=%s from=%s dictionary=%s",
					b.type, b.whereFrom, b.getFullDictionary().serialize()));
		}

		this.whenBlocksAndBytesLabel.setText(
				String.format("made '%d', %d blocks, %d bytes total",
						displayedLog.createdWhen, displayedLog.blocks.size(), tbytes));
		this.toolStuffLabel.setText(String.format("version=%d, log uid=%d",
				displayedLog.version(), displayedLog.jvm_unique_id ));

		this.referenceLabel.setText(
				String.format("log has reference %s",
						displayedLog.getReference()
				));

		this.blockList.setListData(blockList.toArray(new String[0]));

		//TODO save loc position?
//		int loc = this.jScrollPaneForJsonArea.getVerticalScrollBar().getValue();
		this.jsonStringTextArea.setText(displayedLog.getFullDictionary().print());
		this.jScrollPaneForJsonArea.getVerticalScrollBar().setValue(jsonAreaLoc);
	}

	@Override
	public void setupDisplay() {
		reDisplay();
		this.saveFile.addActionListener(this);

		if (this.displayedLog.getReference() != null) {
			Debug.info("temp: %s", this.displayedLog.getReference().tempPath());
			Debug.info("load: %s", this.displayedLog.getReference().loadPath());
		} else {
			Debug.info("null LogReference.");
		}
	}

	private int getJsonLoc() {
		return this.jScrollPaneForJsonArea.getVerticalScrollBar().getValue();
	}

	public DefaultView() {
		initComponents();
		DefaultCaret caret = (DefaultCaret) this.jsonStringTextArea.getCaret();
		caret.setUpdatePolicy(DefaultCaret.NEVER_UPDATE);

		this.addComponentListener(new ComponentListener(){

			@Override
			public void componentResized(ComponentEvent e) { }

			@Override
			public void componentMoved(ComponentEvent e) { }

			@Override
			public void componentShown(ComponentEvent e) {
				reDisplay();
			}

			@Override
			public void componentHidden(ComponentEvent e) {
				jsonAreaLoc = getJsonLoc();
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

        classAndHostLabel = new javax.swing.JLabel();
        whenBlocksAndBytesLabel = new javax.swing.JLabel();
        toolStuffLabel = new javax.swing.JLabel();
        referenceLabel = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        blockList = new javax.swing.JList<>();
        jScrollPaneForJsonArea = new javax.swing.JScrollPane();
        jsonStringTextArea = new javax.swing.JTextArea();
        saveFile = new javax.swing.JButton();

        classAndHostLabel.setText("class, from name@addr (type)");

        whenBlocksAndBytesLabel.setText("made when, total blocks, total bytes");

        toolStuffLabel.setText("nbtool version, log uid");

        referenceLabel.setText("reference info and paths");

        blockList.setBorder(javax.swing.BorderFactory.createTitledBorder(new javax.swing.border.LineBorder(new java.awt.Color(0, 0, 0), 1, true), "blocks"));
        blockList.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        jScrollPane1.setViewportView(blockList);

        jsonStringTextArea.setEditable(false);
        jsonStringTextArea.setColumns(20);
        jsonStringTextArea.setFont(new java.awt.Font("Monospaced", 0, 13)); // NOI18N
        jsonStringTextArea.setLineWrap(true);
        jsonStringTextArea.setRows(5);
        jsonStringTextArea.setWrapStyleWord(true);
        jsonStringTextArea.setBorder(javax.swing.BorderFactory.createTitledBorder("full json serialization"));
        jScrollPaneForJsonArea.setViewportView(jsonStringTextArea);

        saveFile.setText("save to file");

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPaneForJsonArea, javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 547, Short.MAX_VALUE)
                    .addComponent(classAndHostLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(whenBlocksAndBytesLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(referenceLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(toolStuffLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(saveFile)
                        .addGap(0, 0, Short.MAX_VALUE)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(classAndHostLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(whenBlocksAndBytesLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(toolStuffLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(referenceLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPaneForJsonArea, javax.swing.GroupLayout.DEFAULT_SIZE, 156, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(saveFile)
                .addContainerGap())
        );
    }// </editor-fold>


    // Variables declaration - do not modify
    private javax.swing.JList<String> blockList;
    private javax.swing.JLabel classAndHostLabel;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPaneForJsonArea;
    private javax.swing.JTextArea jsonStringTextArea;
    private javax.swing.JLabel referenceLabel;
    private javax.swing.JButton saveFile;
    private javax.swing.JLabel toolStuffLabel;
    private javax.swing.JLabel whenBlocksAndBytesLabel;
    // End of variables declaration
}
