/**
 * @author Nicholas Dunn
 * @date   05/09/2008
 *
 * An interface for classes that change over time and where an ID must be
 * queried to determine if an action is valid at that given time.
 *
 * For instance, in our classification system we only allow undoing actions
 * that are visible in the current frame.
 */

public interface ID_Queryable {
    public int getID();
}